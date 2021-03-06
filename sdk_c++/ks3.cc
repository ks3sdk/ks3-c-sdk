/*****************************************************************
 *
 *  Copyright (c) 2020 KingSoft.com, Inc. All Rights Reserved
 *
 *****************************************************************/

/**
 * @file: ks3.cc
 * @author: lihaibing(lihaibing@kingsoft.com)
 * @date: 2020-05-16 17:10:19
 * @brief: 
 *
 */

#include "ks3.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "md5.h"
#include "util.h"

namespace ks3 {
namespace sdk {

static const int kBaseBackupTimeMs = 250;

// headers
static const std::string kHost = "Host";
static const std::string kDate = "Date";
static const std::string kContentLength = "Content-Length";
static const std::string kRange = "Range";
static const std::string kContentMD5 = "Content-MD5";

// parameters
static const std::string kUploads = "uploads";
static const std::string kUploadId = "uploadId";
static const std::string kPartNum = "partNumber";
static const std::string kETag = "ETag";

static const std::string kCrr = "crr";
static const std::string kTargetBucket = "targetBucket";
static const std::string kPrefix = "prefix";
static const std::string kDeleteMarkerStatus = "DeleteMarkerStatus";

static unsigned int CalHashCode(const std::string& full_path) {
    unsigned int hashcode = 0;
    for (unsigned int i = 0; i < full_path.size(); ++i) {
        hashcode = hashcode * 31 + full_path.at(i);
    }

    return hashcode;
}

static int SleepMs(int64_t milli_seconds) {
    struct timespec ts;
    ts.tv_sec = milli_seconds / 1000;
    ts.tv_nsec = (milli_seconds % 1000) * 1000000;
    return nanosleep(&ts, NULL);
}

KS3Client::KS3Client(const std::string& host, int max_curl_sessions, bool use_https) {
    host_ = host;
    max_curl_sessions_ = max_curl_sessions;
    use_https_ = use_https;
}

KS3Client::~KS3Client() {
}

int KS3Client::Init() {
    for (int i = 0; i < max_curl_sessions_; ++i) {
        CurlManagerPtr tmp(new CURLManager(host_, use_https_));
        if (tmp->Init() != 0) {
            return -1;
        }

        curl_managers_.push_back(tmp);
    }

    return 0;
}

static std::string GetDate() {
	char date[64] = { '\0' };
	time_t now = time(NULL);
	strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));
    std::string date_str(date);
    return date_str;
}

static bool ShouldRetry(int code, KS3Response* res) {
    if (code == CURLE_OPERATION_TIMEDOUT) {
        return true;
    }

    if (code == CURLE_OK && (res->status_code == 409 || res->status_code >= 500)) {
        // 409: concurrent operation confilict
        // 500: server internal error
        return true;
    }

    return false;
}

int KS3Client::Call(MethodType type, unsigned int index, KS3Context& ctx, KS3Response* result) {
    int retry_times = 0;
    int code = CURLE_OK;
    int sleepms = kBaseBackupTimeMs;

    while (true) {
        if (type == GET_METHOD) {
            code = curl_managers_.at(index)->Get(ctx, result);
        } else if (type == PUT_METHOD) {
            code = curl_managers_.at(index)->Put(ctx, result);
        } else if (type == POST_METHOD) {
            code = curl_managers_.at(index)->Post(ctx, result);
        } else if (type == DELETE_METHOD) {
            code = curl_managers_.at(index)->Delete(ctx, result);
        } else if (type == HEAD_METHOD) {
            code = curl_managers_.at(index)->Head(ctx, result);
        } else {
            return -1;
        }

        retry_times++;
        if (retry_times >= 3 || !ShouldRetry(code, result)) {
            break;
        }
        ctx.uploaded_len = 0;

        result->Reset();
        if (sleepms > 1000) {
            sleepms = 1000;
        }
        SleepMs(sleepms);
        sleepms *= 2;
    }

    return code;
}

void KS3Client::BuildCommContext(const ClientContext& context, unsigned int* index, KS3Context* ctx) {
    unsigned int hashcode = CalHashCode(context.object_key);
    *index = hashcode % max_curl_sessions_;

    ctx->bucket = context.bucket;
    ctx->object_key = context.object_key;
    ctx->accesskey = context.accesskey;
    ctx->secretkey = context.secretkey;

    ctx->headers.insert(std::pair<std::string, std::string>(kDate, GetDate()));
    ctx->headers.insert(std::pair<std::string, std::string>(kHost, host_));
}

int KS3Client::UploadObject(const ClientContext& context, const char* buffer, int buffer_size, KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);
    ctx.headers.insert(std::pair<std::string, std::string>(kContentLength, std::to_string(buffer_size)));
    ctx.data = buffer;
    ctx.data_len = buffer_size;

    return Call(PUT_METHOD, index, ctx, response);
}

int KS3Client::GetObject(const ClientContext& context, KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);
    if (context.start_offset >= 0 && context.end_offset >= 0 && context.end_offset >= context.start_offset) {
        // add range header
        char tmp[128] = { '\0' };
        snprintf(tmp, 128, "bytes=%ld-%ld", context.start_offset, context.end_offset);
        ctx.headers.insert(std::pair<std::string, std::string>(kRange, tmp));
        response->content.reserve(context.end_offset - context.start_offset + 2);
    } else {
        // default object size as 16MB
        response->content.reserve(16 * 1024 * 1024);
    }

    return Call(GET_METHOD, index, ctx, response);
}

int KS3Client::DeleteObject(const ClientContext& context, KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);

    return Call(DELETE_METHOD, index, ctx, response);
}

int KS3Client::HeadObject(const ClientContext& context, KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);

    return Call(HEAD_METHOD, index, ctx, response);
}

int KS3Client::InitMultipartUpload(const ClientContext& context, KS3Response* response, std::string* uploadId) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);
    ctx.parameters.insert(std::pair<std::string, std::string>(kUploads, ""));
    ctx.headers.insert(std::pair<std::string, std::string>("Content-Type", "text/plain;charset=UTF-8"));

    int code = Call(POST_METHOD, index, ctx, response);
    if (code == CURLE_OK && response->status_code == 200) {
        xmlDocPtr doc = NULL;
        xmlNodePtr proot = NULL;
        xmlNodePtr cur = NULL;
        xmlKeepBlanksDefault(0);
        doc = xmlReadMemory(response->content.data(), response->content.size(), NULL, "UTF-8", XML_PARSE_RECOVER);
        if (doc != NULL) {
            proot = xmlDocGetRootElement(doc);
            if (proot != NULL) {
                cur = proot->xmlChildrenNode;
                while (cur != NULL) {
                    if (xmlStrcmp(cur->name, (const xmlChar*)("UploadId")) == 0) {
                        xmlChar* tmp_content = xmlNodeGetContent(cur);
                        (*uploadId).assign((char*)tmp_content);
                        xmlFree(tmp_content);
                        break;
                    }

                    cur = cur->next;
                }
            }
            xmlFreeDoc(doc);
            xmlCleanupParser();
            xmlMemoryDump();
        }

    }

    return code;
}

int KS3Client::UploadPart(const ClientContext& context, const char* buffer, int buffer_size, KS3Response* response,
                          std::string* etag) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);
    ctx.headers.insert(std::pair<std::string, std::string>(kContentLength, std::to_string(buffer_size)));
    ctx.data = buffer;
    ctx.data_len = buffer_size;
    ctx.parameters.insert(std::pair<std::string, std::string>(kUploadId, context.uploadId));
    ctx.parameters.insert(std::pair<std::string, std::string>(kPartNum, std::to_string(context.partNum)));

    int code = Call(PUT_METHOD, index, ctx, response);
    if (code == CURLE_OK && response->status_code == 200) {
        std::map<std::string, std::string>::iterator it = response->res_headers.find(kETag);
        if (it != response->res_headers.end()) {
            (*etag).assign(it->second);
        }
    }

    return code;
}

int KS3Client::CompleteMultipartUpload(const ClientContext& context, const std::map<int, std::string>& parts,
                                       KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);
    ctx.parameters.insert(std::pair<std::string, std::string>(kUploadId, context.uploadId));
    ctx.headers.insert(std::pair<std::string, std::string>("Content-Type", "text/plain;charset=UTF-8"));

    // build xml content
    xmlDocPtr doc = xmlNewDoc((const xmlChar*)"1.0");
    xmlNodePtr root = xmlNewNode(NULL, (const xmlChar*)"CompleteMultipartUpload");
    xmlDocSetRootElement(doc, root);
    std::map<int, std::string>::const_iterator it = parts.cbegin();
    for (; it != parts.cend(); ++it) {
        xmlNodePtr child = xmlNewNode(NULL, (const xmlChar*)"Part");

        xmlNodePtr part = xmlNewNode(NULL, (const xmlChar*)"PartNumber");
        char tmp[64];
        snprintf(tmp, 64, "%d", it->first);
        xmlNodePtr part_content = xmlNewText((const xmlChar*)(tmp));
        xmlAddChild(part, part_content);

        xmlNodePtr etag = xmlNewNode(NULL, (const xmlChar*)kETag.c_str());
        xmlNodePtr etag_content = xmlNewText((const xmlChar*)(it->second.c_str()));
        xmlAddChild(etag, etag_content);

        xmlAddChild(child, part);
        xmlAddChild(child, etag);
        xmlAddChild(root, child);
    }

    xmlChar* out = NULL;
    int len = 0;
    xmlDocDumpFormatMemory(doc, &out, &len, 1);

    std::string content;
    content.assign((char*)out);
    xmlFree(out);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();

    ctx.data = content.data();
    ctx.data_len = content.size();

    return Call(POST_METHOD, index, ctx, response);
}

int KS3Client::AbortMultipartUpload(const ClientContext& context, KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);
    ctx.parameters.insert(std::pair<std::string, std::string>(kUploadId, context.uploadId));

    return Call(DELETE_METHOD, index, ctx, response);
}

int KS3Client::ListParts(const ClientContext& context, std::map<int, std::string>* parts, KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);
    ctx.parameters.insert(std::pair<std::string, std::string>(kUploadId, context.uploadId));

    int code = Call(GET_METHOD, index, ctx, response);
    if (code == CURLE_OK && response->status_code == 200) {
        xmlDocPtr doc = NULL;
        xmlNodePtr proot = NULL;
        xmlNodePtr cur = NULL;
        xmlKeepBlanksDefault(0);
        doc = xmlReadMemory(response->content.data(), response->content.size(), NULL, "UTF-8", XML_PARSE_RECOVER);
        if (doc != NULL) {
            proot = xmlDocGetRootElement(doc);
            if (proot != NULL) {
                cur = proot->xmlChildrenNode;
                while (cur != NULL) {
                    if (xmlStrcmp(cur->name, (const xmlChar*)("Part")) == 0) {
                        xmlNodePtr child_cur = cur->xmlChildrenNode;
                        int part_number = -1;
                        std::string etag_tmp;
                        while (child_cur != NULL) {
                            if (xmlStrcmp(child_cur->name, (const xmlChar*)("PartNumber")) == 0) {
                                xmlChar* tmp_content = xmlNodeGetContent(child_cur);
                                part_number = atoi((char*)tmp_content);
                                xmlFree(tmp_content);
                            }

                            if (xmlStrcmp(child_cur->name, (const xmlChar*)(kETag.c_str())) == 0) {
                                xmlChar* tmp_content = xmlNodeGetContent(child_cur);
                                etag_tmp.assign((char*)tmp_content);
                                xmlFree(tmp_content);
                            }

                            child_cur = child_cur->next;
                        }

                        parts->insert(std::pair<int, std::string>(part_number, etag_tmp));
                    }

                    cur = cur->next;
                }
            }
            xmlFreeDoc(doc);
            xmlCleanupParser();
            xmlMemoryDump();
        }
    }

    return code;
}

int KS3Client::SetReplication(const ClientContext& context, const ReplicationInfo& crr_ctx,
                              KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);
    ctx.parameters.insert(std::pair<std::string, std::string>(kCrr, ""));
    ctx.headers.insert(std::pair<std::string, std::string>("Content-Type", "application/xml;charset=UTF-8"));

    // build xml content
    xmlDocPtr doc = xmlNewDoc((const xmlChar*)"1.0");
    xmlNodePtr root = xmlNewNode(NULL, (const xmlChar*)"Replication");
    xmlDocSetRootElement(doc, root);
    xmlNewProp(root, (const xmlChar*)"xmlns", (const xmlChar*)"http://s3.amazonaws.com/doc/2006-03-01/");

    std::vector<std::string>::const_iterator it = crr_ctx.prefixes.cbegin();
    for (; it != crr_ctx.prefixes.cend(); ++it) {
        xmlNodePtr prefix = xmlNewNode(NULL, (const xmlChar*)kPrefix.c_str());
        xmlNodePtr content = xmlNewText((const xmlChar*)((*it).c_str()));
        xmlAddChild(prefix, content);

        xmlAddChild(root, prefix);
    }

    // add deletemarkerstatus
    if (crr_ctx.enable_delete_replication) {
        xmlNodePtr delete_marker_status = xmlNewNode(NULL, (const xmlChar*)kDeleteMarkerStatus.c_str());
        xmlNodePtr delete_content = xmlNewText((const xmlChar*)("Enabled"));
        xmlAddChild(delete_marker_status, delete_content);
        xmlAddChild(root, delete_marker_status);
    }

    // add targetBucket
    xmlNodePtr target_bucket = xmlNewNode(NULL, (const xmlChar*)kTargetBucket.c_str());
    xmlNodePtr target_content = xmlNewText((const xmlChar*)(crr_ctx.target_bucket.c_str()));
    xmlAddChild(target_bucket, target_content);
    xmlAddChild(root, target_bucket);

    xmlChar* out = NULL;
    int len = 0;
    xmlDocDumpFormatMemory(doc, &out, &len, 1);
    //xmlDocDumpMemoryEnc(doc, &out, &len, "UTF-8");

    std::string content;
    content.assign((char*)out);

    xmlFree(out);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    xmlMemoryDump();

    ctx.data = content.data();
    ctx.data_len = content.size();
    ctx.headers.insert(std::pair<std::string, std::string>(kContentLength, std::to_string(ctx.data_len)));

    // calculate md5
    const int md5size = 16;
    unsigned char md5_value[md5size] = { '\0' };
    MD5_CTX md5;
    MD5Init(&md5);
    MD5Update(&md5, (unsigned char*)content.data(), ctx.data_len);
    MD5Final(&md5, md5_value);
    // encode md5 to base64
	char b64[((md5size + 1) * 4) / 3+1] = { '\0' };
	int b64Len = base64Encode(md5_value, md5size, b64);
	b64[b64Len] = '\0';
    ctx.headers.insert(std::pair<std::string, std::string>(kContentMD5, b64));

    return Call(PUT_METHOD, index, ctx, response);
}

int KS3Client::GetReplication(const ClientContext& context, ReplicationInfo* crr_ctx,
                              KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);
    ctx.parameters.insert(std::pair<std::string, std::string>(kCrr, ""));
    crr_ctx->enable_delete_replication = false;

    int code = Call(GET_METHOD, index, ctx, response);
    if (code == CURLE_OK && response->status_code == 200) {
        xmlDocPtr doc = NULL;
        xmlNodePtr proot = NULL;
        xmlKeepBlanksDefault(0);
        doc = xmlReadMemory(response->content.data(), response->content.size(), NULL, "UTF-8", XML_PARSE_RECOVER);
        if (doc != NULL) {
            proot = xmlDocGetRootElement(doc);
            if (proot != NULL) {
                xmlNodePtr child_cur = proot->xmlChildrenNode;
                while (child_cur != NULL) {
                    if (xmlStrcmp(child_cur->name, (const xmlChar*)(kPrefix.c_str())) == 0) {
                        xmlChar* tmp_content = xmlNodeGetContent(child_cur);
                        crr_ctx->prefixes.push_back((char*)tmp_content);
                        xmlFree(tmp_content);
                    }

                    if (xmlStrcmp(child_cur->name, (const xmlChar*)(kDeleteMarkerStatus.c_str())) == 0) {
                        xmlChar* tmp_content = xmlNodeGetContent(child_cur);
                        std::string tmp_status;
                        tmp_status.assign((char*)tmp_content);
                        if (tmp_status == "Enabled") {
                            crr_ctx->enable_delete_replication = true;
                        }
                        xmlFree(tmp_content);
                    }

                    if (xmlStrcmp(child_cur->name, (const xmlChar*)(kTargetBucket.c_str())) == 0) {
                        xmlChar* tmp_content = xmlNodeGetContent(child_cur);
                        crr_ctx->target_bucket.assign((char*)tmp_content);
                        xmlFree(tmp_content);
                    }

                    child_cur = child_cur->next;
                }

            }
        }

        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
    }

    return code;
}

int KS3Client::DeleteReplication(const ClientContext& context, KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);
    ctx.parameters.insert(std::pair<std::string, std::string>(kCrr, ""));

    return Call(DELETE_METHOD, index, ctx, response);
}

}
}
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

/*
 * Copyright (c) 2020 KingSoft.com, Inc. All Rights Reserved
 *
 * @file curl_manager.cc
 * @author lihaibing(lihaibing@kingsoft.com)
 * @date 2020/05/17 20:33:07
 * @brief
 *
 *
*/

#include "curl_manager.h"
#include <time.h>
#include <algorithm>
#include <cctype>
#include "util.h"


namespace ks3 {
namespace sdk {

static const std::string kHTTP = "HTTP";

static const char* SUB_RESOURCES[16]={"acl", "lifecycle", "location", "logging", "notification", "partNumber", \
	"policy", "requestPayment", "torrent", "uploadId", "uploads", "versionId", \
	"versioning", "versions", "website", "crr"};

static const char* RESPONSE_OVERIDES[6]={"response-content-type", "response-content-language", \
	"response-expires", "response-cache-control", \
	"response-content-disposition", "response-content-encoding"};

CURLManager::CURLManager(const std::string& host, bool use_https) {
    use_https_ = use_https;
    host_ = host;

    for (int i = 0; i < 16; i++) {
        ks3_resources_.insert(SUB_RESOURCES[i]);
    }

    for (int i = 0; i < 6; i++) {
        ks3_resources_.insert(RESPONSE_OVERIDES[i]);
    }
}

int CURLManager::Init() {
    handler_ = curl_easy_init();
    if (NULL == handler_) {
        return -1;
    }

    curl_easy_setopt(handler_, CURLOPT_CONNECTTIMEOUT, 1L);
    curl_easy_setopt(handler_, CURLOPT_TIMEOUT_MS, 60 * 1000);

    return 0;
}

void CURLManager::SetMethod(MethodType method_type) {
	switch(method_type) {
	case PUT_METHOD:
		curl_easy_setopt(handler_, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(handler_, CURLOPT_PUT, 1L);
		break;
    case POST_METHOD:
        curl_easy_setopt(handler_, CURLOPT_POST, 1L);
		break;
	case DELETE_METHOD:
		curl_easy_setopt(handler_, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
	case HEAD_METHOD:
		curl_easy_setopt(handler_, CURLOPT_NOBODY, 1L);
        break;
    default:
        break;
	}
}

static size_t upload_data(void *ptr, size_t size, size_t nmemb, void *buf) {
	KS3Context* ctx = (KS3Context*) buf;
	if (ctx->uploaded_len >= ctx->data_len) {
		return 0;
	}

	unsigned int to_read_len = nmemb * size;
    if (to_read_len == 0) {
        return 0;
    }
    
    unsigned int remain_len = ctx->data_len - ctx->uploaded_len;
    if (to_read_len > remain_len) {
        to_read_len = remain_len;
    }

	memcpy(ptr, ctx->data + ctx->uploaded_len, to_read_len);
    ctx->uploaded_len += to_read_len;

	return to_read_len;
}

static size_t read_ks3_headers(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t num_bytes = size * nmemb;
    if (num_bytes <= 0) {
        return 0;
    }

    KS3Response* res = (KS3Response*)stream;
    res->headers_buffer.append((char*)ptr, num_bytes);
    return num_bytes;
}

static size_t read_ks3_response(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t num_bytes = size * nmemb;
    if (num_bytes <= 0) {
        return 0;
    }

    KS3Response* res = (KS3Response*)stream;
    res->content.append((char*)ptr, num_bytes);
    return num_bytes;
}

static std::string kHeaderDelimiter = "\r\n";

static void Trim(std::string& value) {
    // trim space
    size_t tmp_space_pos = value.find_first_not_of(" ");
    if (tmp_space_pos != std::string::npos) {
        value.erase(0, tmp_space_pos);
    }
    tmp_space_pos = value.find_last_not_of(" ");
    if (tmp_space_pos != std::string::npos) {
        value.erase(tmp_space_pos + 1);
    }
}

void CURLManager::ParseResponseHeaders(KS3Response* response) {
    if (handler_ == NULL || response == NULL) {
        return;
    }

    // 1. get http status code
    curl_easy_getinfo(handler_, CURLINFO_RESPONSE_CODE, &(response->status_code));

    // 2. get http status msg
	// 2.1 get first line: start with 'HTTP'
    size_t start_pos = 0;
    size_t end_pos = response->headers_buffer.find(kHeaderDelimiter);
    while (end_pos != std::string::npos) {
        std::string tmp_str = response->headers_buffer.substr(start_pos, end_pos);
        if (tmp_str.find(kHTTP) == 0) {
            // split "HTTP/xxxx status_code status_msg" by space
            size_t space_pos = tmp_str.find_last_of(" ");
            if (space_pos != std::string::npos) {
                // copy status msg
                response->status_msg.append(tmp_str.substr(space_pos + 1));
            }
        } else {
            // parse all headers
            size_t tmp_space = tmp_str.find(":");
            if (tmp_space != std::string::npos) {
                std::string header_name;
                header_name.assign(tmp_str.substr(0, tmp_space));
                Trim(header_name);

                std::string header_value;
                header_value.assign(tmp_str.substr(tmp_space + 1));
                Trim(header_value);

                response->res_headers.insert(std::pair<std::string, std::string>(header_name, header_value));
            }
        }

        start_pos = end_pos + kHeaderDelimiter.size();
        end_pos = response->headers_buffer.find(kHeaderDelimiter, start_pos);
    }
}

static std::string MethodToStr(MethodType method) {
    std::string result;
	switch(method) {
	case PUT_METHOD:
        result = "PUT";
		break;
	case GET_METHOD:
        result = "GET";
		break;
    case POST_METHOD:
        result = "POST";
		break;
	case DELETE_METHOD:
        result = "DELETE";
        break;
	case HEAD_METHOD:
        result = "HEAD";
        break;
    default:
        break;
    }
    return result;
}

void CURLManager::GetQueryString(const KS3Context& ctx, bool only_ks3_resources, std::string* result) {
    result->append("/");
    result->append(ctx.bucket);
    if (!ctx.object_key.empty()) {
        if (ctx.object_key.at(0) != '/') {
            result->append("/");
        }
        result->append(ctx.object_key);
    } else {
        result->append("/");
    }

    bool first_par = true;
    if (!ctx.parameters.empty()) {
        std::map<std::string, std::string>::const_iterator mit = ctx.parameters.cbegin();
        for (; mit != ctx.parameters.cend(); ++mit) {
            if (only_ks3_resources && ks3_resources_.find(mit->first) == ks3_resources_.end()) {
                continue;
            }
            if (first_par) {
                result->append("?");
                first_par = false;
            } else {
                result->append("&");
            }
            result->append(mit->first);
            if (!(mit->second.empty())) {
                result->append("=");
                result->append(mit->second);
            }
        }
    }
}

void CURLManager::GetStringForSign(MethodType method, const KS3Context& ctx, std::string* str_for_sign) {
    std::map<std::string, std::string> canonical_headers;
    std::map<std::string, std::string>::const_iterator it = ctx.headers.cbegin();
    for (; it != ctx.headers.cend(); ++it) {
        std::string header = it->first;
        std::string new_header;
        new_header.assign(header);
        std::transform(new_header.begin(), new_header.end(), new_header.begin(), tolower);
        canonical_headers.insert(std::pair<std::string, std::string>(new_header, it->second));
    }

    // 1. append method
    str_for_sign->clear();
    str_for_sign->append(MethodToStr(method));
    str_for_sign->append("\n");

    // 2. append content md5
    std::map<std::string, std::string>::iterator tit = canonical_headers.find("content-md5");
    if (tit != canonical_headers.end()) {
        str_for_sign->append(tit->second);
    }
    str_for_sign->append("\n");

    // 3. append content type
    tit = canonical_headers.find("content-type");
    if (tit != canonical_headers.end()) {
        str_for_sign->append(tit->second);
    }
    str_for_sign->append("\n");

    // 4. append date
    tit = canonical_headers.find("date");
    if (tit != canonical_headers.end()) {
        str_for_sign->append(tit->second);
    }
    str_for_sign->append("\n");

    // 5. append headers start with x-kss
    tit = canonical_headers.begin();
    for (; tit != canonical_headers.end(); ++tit) {
        if (tit->first.find("x-kss-") == 0) {
            str_for_sign->append(tit->first);
            str_for_sign->append(":");
            str_for_sign->append(tit->second);
            str_for_sign->append("\n");
        }
    }

    // 6. append canonical resources
    std::string canonical_resources;
    canonical_resources.reserve(1024);
    GetQueryString(ctx, true, &canonical_resources);
    str_for_sign->append(canonical_resources);
}

static void ComputeSignature(const std::string& str_for_sign, const std::string& secret_key, std::string* result) {
	unsigned char hmac[20];
	int b64Len = 0;
	char b64[((20 + 1) * 4) / 3+1] = { '\0' };
	HMAC_SHA1(hmac, (const unsigned char*)secret_key.c_str(), secret_key.size(),
             (const unsigned char*)str_for_sign.c_str(), str_for_sign.size());
	b64Len = base64Encode(hmac, 20, b64);
	b64[b64Len] = '\0';
    result->assign(b64);
}

int CURLManager::Call(MethodType method, const KS3Context& ctx, KS3Response* response) {
    ScopedLocker<MutexLock> lock(lock_);

    curl_easy_reset(handler_);
    curl_easy_setopt(handler_, CURLOPT_CONNECTTIMEOUT, 1L);
    curl_easy_setopt(handler_, CURLOPT_TIMEOUT_MS, 60 * 1000);
    // 1. construct url
    std::string url;
    url.reserve(512);
    if (use_https_) {
        url.append("https://");
    } else {
        url.append("http://");
    }
    url.append(host_);
    GetQueryString(ctx, false, &url);

    // get string for sign
    std::string str_for_sign;
    GetStringForSign(method, ctx, &str_for_sign);
    // calculate signature
    std::string signature;
    ComputeSignature(str_for_sign, ctx.secretkey, &signature);

    // auth header;
    std::string auth_header;
    auth_header.append("Authorization:KSS ");
    auth_header.append(ctx.accesskey);
    auth_header.append(":");
    auth_header.append(signature);

    // 2. set url
	curl_slist* http_header = NULL;

    // 3. set headers
    std::map<std::string, std::string>::const_iterator it = ctx.headers.cbegin();
    for (; it != ctx.headers.cend(); ++it) {
        std::string header = it->first;
        header.append(":");
        header.append(it->second);
        http_header = curl_slist_append(http_header, header.c_str());
    }
    // append auth header
    http_header = curl_slist_append(http_header, auth_header.c_str());

    // 4. set op
    if (use_https_) {
        curl_easy_setopt(handler_, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(handler_, CURLOPT_SSL_VERIFYHOST, 2L);
    }

    curl_easy_setopt(handler_, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(handler_, CURLOPT_URL, url.c_str());
    SetMethod(method);
    curl_easy_setopt(handler_, CURLOPT_HTTPHEADER, http_header);

    if (method == POST_METHOD) {
        curl_easy_setopt(handler_, CURLOPT_POSTFIELDS, ctx.data);
        curl_easy_setopt(handler_, CURLOPT_POSTFIELDSIZE, ctx.data_len);
    }

    if (ctx.data != NULL) {
        // put data to ks3
        // set read content func
        curl_easy_setopt(handler_, CURLOPT_READFUNCTION, upload_data);
        curl_easy_setopt(handler_, CURLOPT_READDATA, &ctx);

        curl_easy_setopt(handler_, CURLOPT_INFILE, &ctx);
        curl_easy_setopt(handler_, CURLOPT_INFILESIZE_LARGE, ctx.data_len);
    }

    // set read header callback func
    curl_easy_setopt(handler_, CURLOPT_HEADERFUNCTION, read_ks3_headers);
    curl_easy_setopt(handler_, CURLOPT_HEADERDATA, response);
    // set read body callback func
    curl_easy_setopt(handler_, CURLOPT_WRITEFUNCTION, read_ks3_response);
    curl_easy_setopt(handler_, CURLOPT_WRITEDATA, response);

	CURLcode res = curl_easy_perform(handler_);
    if (res == CURLE_OK) {
        ParseResponseHeaders(response);
    }

	curl_slist_free_all(http_header);
    return res;
}

int CURLManager::Put(const KS3Context& ctx, KS3Response* response) {
    return Call(PUT_METHOD, ctx, response);
}

int CURLManager::Post(const KS3Context& ctx, KS3Response* response) {
    return Call(POST_METHOD, ctx, response);
}

int CURLManager::Get(const KS3Context& ctx, KS3Response* response) {
    return Call(GET_METHOD, ctx, response);
}

int CURLManager::Delete(const KS3Context& ctx, KS3Response* response) {
    return Call(DELETE_METHOD, ctx, response);
}

int CURLManager::Head(const KS3Context& ctx, KS3Response* response) {
    return Call(HEAD_METHOD, ctx, response);
}

}
}
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

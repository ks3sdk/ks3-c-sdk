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

namespace ks3 {
namespace sdk {

// headers
static const std::string kHost = "Host";
static const std::string kDate = "Date";
static const std::string kContentLength = "Content-Length";
static const std::string kRange = "Range";

static unsigned int CalHashCode(const std::string& full_path) {
    unsigned int hashcode = 0;
    for (unsigned int i = 0; i < full_path.size(); ++i) {
        hashcode = hashcode * 31 + full_path.at(i);
    }

    return hashcode;
}

KS3Client::KS3Client(const std::string& host, int max_curl_sessions) {
    host_ = host;
    max_curl_sessions_ = max_curl_sessions;
}

KS3Client::~KS3Client() {
}

int KS3Client::Init() {
    for (int i = 0; i < max_curl_sessions_; ++i) {
        CurlManagerPtr tmp(new CURLManager(host_));
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

    return curl_managers_.at(index)->Put(ctx, response);
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

    return curl_managers_.at(index)->Get(ctx, response);
}

int KS3Client::DeleteObject(const ClientContext& context, KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);

    return curl_managers_.at(index)->Delete(ctx, response);
}

int KS3Client::HeadObject(const ClientContext& context, KS3Response* response) {
    unsigned int index = 0;
    KS3Context ctx;
    BuildCommContext(context, &index, &ctx);

    return curl_managers_.at(index)->Head(ctx, response);
}

}
}
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

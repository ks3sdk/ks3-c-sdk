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

namespace ks3 {
namespace sdk {

static const int kHeaderBufferSize = 1024;
static const int kDataBufferSize = 4 * 1024 * 1024;
static const std::string kHTTP = "HTTP";

CURLManager::CURLManager(const std::string& host) {
    host_ = host;
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
    unsigned int expected_size = res->headers_used + num_bytes;
    if (expected_size > res->headers_len) {
        unsigned int new_size = std::max(expected_size, res->headers_len + kHeaderBufferSize);
        res->headers = (char*)realloc(res->headers, new_size);
        res->headers_len = new_size;
    }

    memcpy(res->headers + res->headers_used, (char*)ptr, num_bytes);
    res->headers_used += num_bytes;

    return num_bytes;
}

static size_t read_ks3_response(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t num_bytes = size * nmemb;
    if (num_bytes <= 0) {
        return 0;
    }

    KS3Response* res = (KS3Response*)stream;
    unsigned int expected_size = res->data_used + num_bytes;
    if (expected_size > res->data_len) {
        unsigned int tmp_size = expected_size - res->data_len;
        num_bytes -= tmp_size;
    }

    memcpy(res->data + res->data_used, (char*)ptr, num_bytes);
    res->data_used += num_bytes;

    return num_bytes;
}

void CURLManager::ParseResponseHeaders(KS3Response* response) {
	char* token = NULL;
	char* out_ptr = NULL;
    char* first_space = NULL;
    char* sec_space = NULL;

    if (handler_ == NULL || response == NULL) {
        return;
    }

    // 1. get http status code
    curl_easy_getinfo(handler_, CURLINFO_RESPONSE_CODE, &(response->status_code));
    if (response->headers == NULL) {
        return;
    }

    // 2. get http status msg
	// 2.1 get first line: start with 'HTTP'
	token = strtok_r(response->headers, "\r\n", &out_ptr);
    while (token != NULL) {
        if (strncmp(kHTTP.c_str(), token, strlen(kHTTP.c_str())) == 0) {
            // 2.2 split "HTTP/xxxx status_code status_msg" by space
            first_space = strstr(token, " ");
            if (first_space != NULL) {
                sec_space = strstr(first_space + 1, " ");
                // 2.3 copy status msg
                if (sec_space != NULL) {
                    response->status_msg.append(sec_space + 1);
                }
            }
        } else {
            // parse all headers
            first_space = strstr(token, ":");
            if (first_space != NULL) {
                std::string header_name;
                header_name.assign(token, first_space - token);
                // trim space
                header_name.erase(0, header_name.find_first_not_of(" "));
                header_name.erase(header_name.find_last_not_of(" ") + 1);

                std::string header_value;
                header_value.assign(first_space + 1);
                // trim space
                header_value.erase(0, header_value.find_first_not_of(" "));
                header_value.erase(header_value.find_last_not_of(" ") + 1);

                response->res_headers.insert(std::pair<std::string, std::string>(header_name, header_value));
            }
        }

        first_space = NULL;
        sec_space = NULL;
        token = strtok_r(NULL, "\r\n", &out_ptr);
    }
}


int CURLManager::Call(MethodType method, const KS3Context& ctx, KS3Response* response) {
    ScopedLocker<MutexLock> lock(lock_);

    curl_easy_reset(handler_);
    // 1. construct url
    std::string url;
    url.reserve(512);
    url.append(host_);
    url.append("/");
    url.append(ctx.bucket);
    if (!ctx.path.empty()) {
        if (ctx.path.at(0) != '/') {
            url.append("/");
        }
        url.append(ctx.path);
    }

    // append query string
    bool first_par = true;
    if (!ctx.parameters.empty()) {
        url.append("?");
        std::map<std::string, std::string>::const_iterator it = ctx.parameters.cbegin();
        for (; it != ctx.parameters.cend(); ++it) {
            if (first_par) {
                first_par = false;
            } else {
                url.append("&");
            }
            url.append(it->first);
            if (!(it->second.empty())) {
                url.append("=");
                url.append(it->second);
            }
        }
    }

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

    // 4. set op
    curl_easy_setopt(handler_, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(handler_, CURLOPT_URL, url.c_str());
    SetMethod(method);
    curl_easy_setopt(handler_, CURLOPT_HTTPHEADER, http_header);

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

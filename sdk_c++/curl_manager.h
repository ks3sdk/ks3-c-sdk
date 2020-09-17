/*****************************************************************
 *
 *  Copyright (c) 2020 KingSoft.com, Inc. All Rights Reserved
 *
 *****************************************************************/


/**
 * @file: curl_manager.h
 * @author: lihaibing(lihaibing@kingsoft.com)
 * @date: 2020-05-16 17:10:19
 * @brief: 
 *
 */

#ifndef _KS3_CURL_MANAGER_H_
#define _KS3_CURL_MANAGER_H_

#include <map>
#include <set>
#include <string>
#include <string.h>
#include <memory>
#include "curl/curl.h"
#include "scoped_locker.h"

namespace ks3 {
namespace sdk {

#ifndef DISALLOW_COPY_AND_ASSIGN
    #define DISALLOW_COPY_AND_ASSIGN(TypeName) \
        TypeName(const TypeName&); \
        void operator=(const TypeName&)
#endif

enum MethodType {
    GET_METHOD = 0,
	PUT_METHOD,
	DELETE_METHOD,
	POST_METHOD,
    HEAD_METHOD,
    MethodType_END
};

struct KS3Context {
    std::string bucket;
    std::string object_key;

    std::string accesskey;
    std::string secretkey;

    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> parameters;

    const char* data;
    unsigned int data_len;
    unsigned int uploaded_len;

    KS3Context() {
        data = NULL;
        data_len = 0;
        uploaded_len = 0;
    }

    virtual ~KS3Context() {
    }
};

struct KS3Response {
    int status_code;
    std::string status_msg;
    std::map<std::string, std::string> res_headers;

    std::string headers_buffer;

    std::string content;
    KS3Response() {
        // default http response code
        status_code = 400;
        Reset();
    }

    void Reset() {
        status_msg.clear();
        res_headers.clear();
        headers_buffer.clear();
        headers_buffer.reserve(4096);

        size_t capacity = content.capacity();
        content.clear();
        content.reserve(capacity);
    }

    virtual ~KS3Response() {
    }
};

class CURLManager {
public:
    CURLManager(const std::string& host, bool use_https);
    virtual ~CURLManager() {
        curl_easy_cleanup(handler_);
    }

    int Init();

    int Put(const KS3Context& ctx, KS3Response* response);
    int Post(const KS3Context& ctx, KS3Response* response);
    int Get(const KS3Context& ctx, KS3Response* response);
    int Delete(const KS3Context& ctx, KS3Response* response);
    int Head(const KS3Context& ctx, KS3Response* response);

private:
    void SetMethod(MethodType method_type);
    void ParseResponseHeaders(KS3Response* response);
    int Call(MethodType method, const KS3Context& ctx, KS3Response* response);
    void GetQueryString(const KS3Context& ctx, bool only_ks3_resources, std::string* result);
    void GetStringForSign(MethodType method, const KS3Context& ctx, std::string* str_for_sign);

private:
    DISALLOW_COPY_AND_ASSIGN(CURLManager);
    std::string host_;
    bool use_https_;

    MutexLock lock_;
    CURL* handler_;

    std::set<std::string> ks3_resources_;
};

typedef std::shared_ptr<CURLManager> CurlManagerPtr;

}
}//namespace ks3

#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

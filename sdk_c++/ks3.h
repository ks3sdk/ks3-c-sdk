/*****************************************************************
 *
 *  Copyright (c) 2020 KingSoft.com, Inc. All Rights Reserved
 *
 *****************************************************************/


/**
 * @file: ks3.h
 * @author: lihaibing(lihaibing@kingsoft.com)
 * @date: 2020-05-16 17:10:19
 * @brief: 
 *
 */

#ifndef _KS3_KS3_SDK_H_
#define _KS3_KS3_SDK_H_

#include <vector>
#include <string>
#include "curl_manager.h"

namespace ks3 {
namespace sdk {


struct ClientContext {
    std::string bucket;
    std::string object_key;
    std::string accesskey;
    std::string secretkey;

    // for read
    int64_t start_offset;
    int64_t end_offset;

    // for multiple upload
    std::string uploadId;
    int partNum;

    ClientContext() {
        start_offset = -1;
        end_offset = -1;

        partNum = -1;
    }
};

struct ReplicationInfo {
    // only replicate those objects which start with oneof following prefixes;
    // default is empty, replicate all objects
    std::vector<std::string> prefixes;
    std::string target_bucket;
    // true:replicate delete operation;
    // false: don't replicate delete operation;
    bool enable_delete_replication;

    ReplicationInfo() {
        enable_delete_replication = true;
    }
};

class KS3Client {
public:
    KS3Client(const std::string& host, int max_curl_sessions = 31, bool use_https = false);
    virtual ~KS3Client();

    // should call this when program start up
    static int InitGlobalCurl() {
        return curl_global_init(CURL_GLOBAL_ALL);
    }

    static void DestroyGlobalCurl() {
        curl_global_cleanup();
    }

    int Init();
    int UploadObject(const ClientContext& ctx, const char* buffer, int buffer_size, KS3Response* response);
    int GetObject(const ClientContext& context, KS3Response* response);
    int DeleteObject(const ClientContext& context, KS3Response* response);
    int HeadObject(const ClientContext& context, KS3Response* response);

    // multiple part uploads
    int InitMultipartUpload(const ClientContext& ctx, KS3Response* response, std::string* uploadId);
    int UploadPart(const ClientContext& ctx, const char* buffer, int buffer_size, KS3Response* response,
                   std::string* etag);
    int CompleteMultipartUpload(const ClientContext& ctx, const std::map<int, std::string>& parts, KS3Response* response);
    int AbortMultipartUpload(const ClientContext& ctx, KS3Response* response);
    int ListParts(const ClientContext& ctx, std::map<int, std::string>* parts, KS3Response* response);

    // cross region replication
    int SetReplication(const ClientContext& ctx, const ReplicationInfo& crr_ctx, KS3Response* response);
    int GetReplication(const ClientContext& ctx, ReplicationInfo* crr_ctx, KS3Response* response);
    int DeleteReplication(const ClientContext& ctx, KS3Response* response);

private:
    void BuildCommContext(const ClientContext& context, unsigned int* index, KS3Context* ctx);
    int Call(MethodType type, unsigned int index, KS3Context& ctx, KS3Response* result);

private:
    DISALLOW_COPY_AND_ASSIGN(KS3Client);

    bool use_https_;
    std::string host_;
    int max_curl_sessions_;
    std::vector<CurlManagerPtr> curl_managers_;
};

}
}//namespace ks3

#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

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

class KS3Client {
public:
    KS3Client(const std::string& host, int max_curl_sessions = 997);
    virtual ~KS3Client();

    // should call this when program start up
    static int InitGlobalCurl() {
        return curl_global_init(CURL_GLOBAL_ALL);
    }

    static void DestroyGlobalCurl() {
        curl_global_cleanup();
    }

    int Init();

private:
    DISALLOW_COPY_AND_ASSIGN(KS3Client);

    std::string host_;
    int max_curl_sessions_;
    std::vector<CurlManagerPtr> curl_managers_;
};

}
}//namespace ks3

#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

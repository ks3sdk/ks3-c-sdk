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
    time_t now;
    time(&now);
    struct tm t;
    localtime_r(&now, &t);
    char time_str[64] = {0};
    snprintf(time_str, 64, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", t.tm_year+1900, t.tm_mon + 1, t.tm_mday,
             t.tm_hour, t.tm_min, t.tm_sec);

    std::string date_str(time_str);
    return date_str;
}


}
}
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

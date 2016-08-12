/*************************************************************************
	> File Name: ks3_api_info.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Thu 31 Mar 2016 06:00:54 PM CST
 ************************************************************************/
#ifndef _KS3_C_SDK_TEST_KS3_API_INFO_H_
#define _KS3_C_SDK_TEST_KS3_API_INFO_H_

#include "pandora/thread_cond.h"

namespace ks3_c_sdk
{
namespace test
{

using std::string;
using pandora::CThreadMutex;
using pandora::CThreadCond;

struct Ks3ApiInfo
{
    string host;
    string bucket;
    string access_key;
    string secret_key;
    Ks3ApiInfo& operator=(const Ks3ApiInfo& one) {
        host = one.host;
        bucket = one.bucket;
        access_key = one.access_key;
        secret_key = one.secret_key;
        return *this;
    }
};

class CountDownLatch {
public:
    CountDownLatch(int count) : count_(count) {}
    void CountDown() {
        cond_.Lock();
        count_--;
        if (count_ == 0) {
            cond_.Signal();
        }
        cond_.Unlock();
    }
    void Wait() {
        cond_.Lock();
        while (count_ != 0) {
            cond_.Wait(1000);
        }
        cond_.Unlock();
    }
private:
    int count_;
    CThreadCond cond_;

};

}
}

#endif

/*************************************************************************
	> File Name: press_upload.cpp
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Thu 31 Mar 2016 02:37:40 PM CST
 ************************************************************************/
#ifndef _KS3_C_SDK_TEST_PRESS_COMMON_H_
#define _KS3_C_SDK_TEST_PRESS_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "ks3_api_info.h"

namespace ks3_c_sdk
{
namespace test
{

using std::string;

class Ks3Presser {
public:
    Ks3Presser() {}

    void Init(const Ks3ApiInfo& ks3_api_info,
            const string& src_dir, int seq, CountDownLatch* latch);
    static void* Run(void * arg);
    int Start() {
        //thread_.Start(this, NULL);
        pthread_create(&thread_, 0, Run, (void*)this);
    }
    int Join() {
        pthread_join(thread_,NULL);
    }
    virtual void HandleFile(const string& local_file,
            const string& object_key, int32_t size,
            const string& relative_path) = 0;
private:
    void WalkDir(const string& dir, int depth);

protected:
    int seq_;
    Ks3ApiInfo ks3_api_info_;

private:
    string src_dir_;
    pthread_t thread_;
    CountDownLatch* latch_;
};

}  // end of namespace test
}  // end of namespace ks3_c_sdk

#endif

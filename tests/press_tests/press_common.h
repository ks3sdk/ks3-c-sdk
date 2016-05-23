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
#include "pandora/cthread.h"
#include "ks3_api_info.h"

namespace ks3_c_sdk
{
namespace test
{

using std::string;
using pandora::CThread;
using pandora::Runnable;

class Ks3Presser : public Runnable {
public:
    Ks3Presser() {}

    void Init(const Ks3ApiInfo& ks3_api_info,
            const string& src_dir, int seq, CountDownLatch* latch);
    void Run(CThread * thread, void * arg);
    int Start() {
        thread_.Start(this, NULL);
    }
    int Join() {
        thread_.Join();
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
    CThread thread_;
    CountDownLatch* latch_;
};

}  // end of namespace test
}  // end of namespace ks3_c_sdk

#endif

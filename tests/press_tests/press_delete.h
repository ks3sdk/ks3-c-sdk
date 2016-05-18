/*************************************************************************
	> File Name: press_upload.cpp
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Thu 31 Mar 2016 02:37:40 PM CST
 ************************************************************************/

#ifndef _KS3_C_SDK_TEST_PRESS_DELETE_H_
#define _KS3_C_SDK_TEST_PRESS_DELETE_H_

#include <stdio.h>
#include <stdlib.h>
#include "press_common.h"

namespace ks3_c_sdk
{
namespace test
{

class Ks3Deleter : public Ks3Presser {
public:
    Ks3Deleter() {}
    void HandleFile(const string& local_file,
            const string& object_key, int32_t size,
            const string& relative_path);
};

}  // end of namespace test
}  // end of namespace ks3_c_sdk

#endif

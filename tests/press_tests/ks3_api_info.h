/*************************************************************************
	> File Name: ks3_api_info.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Thu 31 Mar 2016 06:00:54 PM CST
 ************************************************************************/
#ifndef _KS3_C_SDK_TEST_KS3_API_INFO_H_
#define _KS3_C_SDK_TEST_KS3_API_INFO_H_

namespace ks3_c_sdk
{
namespace test
{

using std::string;
struct Ks3ApiInfo
{
    string host;
    string bucket;
    string access_key;                                                                                                
    string secret_key;
};

}
}

#endif

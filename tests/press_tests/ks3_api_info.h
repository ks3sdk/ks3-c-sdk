/*************************************************************************
	> File Name: ks3_api_info.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Thu 31 Mar 2016 06:00:54 PM CST
 ************************************************************************/
#ifndef _KS3_C_SDK_TEST_KS3_API_INFO_H_
#define _KS3_C_SDK_TEST_KS3_API_INFO_H_

#include <pthread.h>
#include <sys/time.h>

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
    CountDownLatch(int count) : count_(count)     {
        pthread_cond_init(&cond_, NULL);
		pthread_mutex_init(&_mutex, NULL);
	}
	~CountDownLatch() {
       pthread_cond_destroy(&cond_);
	   pthread_mutex_destroy(&_mutex);
	}
    void CountDown() {
        #if 0
        cond_.Lock();
        count_--;
        if (count_ == 0) {
            cond_.Signal();
        }
        cond_.Unlock();
		#endif
		pthread_mutex_lock(&_mutex);
		count_--;
        if (count_ == 0) {
            pthread_cond_signal(&cond_);
        }
		pthread_mutex_unlock(&_mutex);
    }
    void Wait() {
        #if 0
        cond_.Lock();
        while (count_ != 0) {
            cond_.Wait(1000);
        }
        cond_.Unlock();
		#endif
		struct timeval now;
		struct timespec outtime;
			
		pthread_mutex_lock(&_mutex);
		while (count_ != 0) {
			gettimeofday(&now, NULL);
			outtime.tv_sec = now.tv_sec + 1;
            pthread_cond_timedwait(&cond_, &_mutex, &outtime);
        }
		pthread_mutex_unlock(&_mutex);
    }
private:
    int count_;
    pthread_cond_t   cond_;
	pthread_mutex_t _mutex;

};

}
}

#endif

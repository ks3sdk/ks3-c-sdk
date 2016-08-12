#ifndef _PRESS_MULTIPARTS_H_
#define _PRESS_MULTIPARTS_H_


#include <stdio.h>
#include <stdlib.h>
#include "press_common.h"
#include <list>

#define ETAG_LEN 32
namespace ks3_c_sdk
{
namespace test
{

struct part_info {
    long offset;
    long size;
    int part_num;
    char etag[ETAG_LEN];
    part_info(){}
    part_info(long off, long sz, int part_no): offset(off), 
        size(sz), part_num(part_no) {}
    friend inline bool operator<(const part_info & one, const part_info &two) {
        return one.part_num < two.part_num;
    }
};

using std::list;

class Ks3Worker : public Runnable {
public:
    Ks3Worker() {
        max_part_size_ = 0;
        thread_=NULL;
        thread_num_ = 0;
        buf_ = NULL;

    }
    ~Ks3Worker() {
        if (buf_) {
            for ( int i = 0; i < thread_num_; ++i) {
                if (buf_[i]) {
                    delete[] buf_[i];
                    buf_[i] = NULL;
                }
            }
            delete[] buf_;
        }

        if (thread_) {
            delete[] thread_;
            thread_ = NULL;
        }

        for ( int i = 0; i < 2; ++i) {
            while(!upload_arr_[i].empty()) {
                delete  upload_arr_[i].front();
                upload_arr_[i].pop_front();
            }                
        }
    }

    int init(const Ks3ApiInfo & info,  const string &filename, 
        const string & object_key, int seq); 

    void Run(CThread *thread, void *arg);
    
    int Start() {
        for (int i = 0; i < thread_num_; i++ ) {
            thread_[i].Start(this, (void*)&buf_[i]);
        }
    }
    
    int Finish();
private:
    int parser_file(const char* filename, int &part_num);
    
    int Join() {
        for (int i = 0; i < thread_num_; i++ )
            thread_[i].Join();
    }

private:
    Ks3ApiInfo ks3Info_;
    string filename_;
    string object_key_;
    string upload_id_;
    int seq_;
    int max_part_size_;
    char **buf_;

    CThread *thread_;
    int thread_num_;
    list<part_info*> upload_arr_[2]; 
    CThreadMutex thread_mutex_[2];
};

class Ks3Multiparts : public Ks3Presser {
public:
    Ks3Multiparts() {}
    void HandleFile(const string& local_file,
            const string& object_key, int32_t size,
            const string& relative_path);

};

}  // end of namespace test
}  // end of namespace ks3_c_sdk


#endif // _PRESS_MULTIPARTS_H_

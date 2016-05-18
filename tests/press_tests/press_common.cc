
#include "press_common.h"
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace ks3_c_sdk
{
namespace test
{

void Ks3Presser::Init(const Ks3ApiInfo& ks3_api_info,
        const string& src_dir, int seq, CountDownLatch* latch) {
    ks3_api_info_.host = ks3_api_info.host;
    ks3_api_info_.bucket = ks3_api_info.bucket;
    ks3_api_info_.access_key = ks3_api_info.access_key;
    ks3_api_info_.secret_key = ks3_api_info.secret_key;
    src_dir_ = src_dir;
    seq_ = seq;
    latch_ = latch;
}

void Ks3Presser::Run(CThread* thread, void* arg) {
    WalkDir(src_dir_, 0);
    latch_->CountDown();
}

void Ks3Presser::WalkDir(const string& src_dir, int depth) {
    DIR* dp = NULL;
    struct dirent* entry = NULL;
    struct stat stat_buf;

    // 1. open dir
    if ((dp = opendir(src_dir.c_str())) == NULL) {
        fprintf(stdout, "[FAIL]seq=%d, cannot open directory:%s, err=%s\n",
                seq_, src_dir.c_str(), strerror(errno));
        return;
    }
    // 2. read dir
    //chdir(src_dir.c_str());
    while ((entry = readdir(dp)) != NULL) {
        string localfile = src_dir + "/" + entry->d_name;
        lstat(localfile.c_str(), &stat_buf);
        if (S_ISDIR(stat_buf.st_mode)) {
            // ignore . and ..
            if (strcmp(".", entry->d_name) == 0
                    || strcmp("..", entry->d_name) == 0) {
                continue;
            }
            printf("seq=%d, dir=%*s%s/\n", seq_, depth, "", entry->d_name);
            string child_dir = src_dir + "/" + entry->d_name;
            WalkDir(child_dir, depth + 4);
        } else {
            printf("seq=%d, file=%*s%s\n", seq_, depth, "", entry->d_name);
            char object_key[100];
            sprintf(object_key, "ks3_c_sdk_dir_144_%d/%s", seq_, entry->d_name);
            HandleFile(localfile, object_key, stat_buf.st_size, entry->d_name);
        }
    }
    // 3. change work dir
    //chdir("..");
    closedir(dp);
}

}  // end of test
}  // end of ks3_c_sdk


#include <map>
#include <vector>
#include <string.h>
#include "api.h"
#include "press_upload.h"
#include "press_upload_buf.h"
#include "press_download.h"
#include "press_delete.h"
#include "press_multiparts.h"

using std::map;
using std::vector;
using std::string;
using ks3_c_sdk::test::Ks3ApiInfo;
using ks3_c_sdk::test::Ks3Uploader;
using ks3_c_sdk::test::Ks3BufUploader;
using ks3_c_sdk::test::Ks3Downloader;
using ks3_c_sdk::test::Ks3Deleter;
using ks3_c_sdk::test::Ks3Presser;
using ks3_c_sdk::test::CountDownLatch;
using ks3_c_sdk::test::Ks3Multiparts;

Ks3Presser* GetKs3Presser(const string& op);

char ak[100];
char sk[100];
int load_key();
int createbucket(const Ks3ApiInfo& info);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("[Usage] %s press_upload [dir]\n", argv[0]);
        printf("[Usage] %s press_upload_buf [dir]\n", argv[0]);
        printf("[Usage] %s press_download [dir]\n", argv[0]);
        printf("[Usage] %s press_delete [dir]\n", argv[0]);
        printf("[Usage] %s press_multiparts [dir]\n", argv[0]);
        return 0;
    }
    string op = argv[1];
    string src_dir = argv[2];
    printf("entry src_dir=%s, op=%s\n",
            src_dir.c_str(), op.c_str());

    int ret = load_key();
    if (ret != 0) {
        return ret;
    }
    Ks3ApiInfo ks3_api_info;
    ks3_api_info.host = "ks3-cn-beijing-internal.ksyun.com";
    ks3_api_info.bucket = "test-c-sdk-bucket1";
    ks3_api_info.access_key = ak;
    ks3_api_info.secret_key = sk;

    ks3_global_init();
    createbucket(ks3_api_info);

    int count = 10;
    CountDownLatch latch(count);
    map<int, Ks3Presser*> presser_map;
    for (int seq = 1; seq <= count; seq++) {
        Ks3Presser* ks3_presser = GetKs3Presser(op);
        ks3_presser->Init(ks3_api_info, src_dir, seq, &latch);
        ks3_presser->Start();
        presser_map[seq] = ks3_presser;
        printf("%s seq=%d started\n", op.c_str(), seq);
    }
    latch.Wait();
    for (int seq = 1; seq <= count; seq++) {
        Ks3Presser* ks3_presser = presser_map[seq];
        ks3_presser->Join();
        delete ks3_presser;
        printf("%s seq=%d exit\n", op.c_str(), seq);
    }

    ks3_global_destroy();
    return 0;
}

Ks3Presser* GetKs3Presser(const string& op) {
    if (op.compare("press_upload") == 0) {
        return new Ks3Uploader;
    } else if (op.compare("press_upload_buf") == 0) {
        return new Ks3BufUploader;
    } else if (op.compare("press_download") == 0) {
        return new Ks3Downloader;
    } else if (op.compare("press_delete") == 0) {
        return new Ks3Deleter;
    } else if (op.compare("press_multiparts") == 0) {
        return new Ks3Multiparts;
    }
    return NULL;
}

int load_key() {
    const char* key_file = "/tmp/key_test_sdk";
    FILE* fp = NULL;
    fp = fopen(key_file, "r");
    if (fp == NULL) {
        printf("fopen file=%s failed\n", key_file);
        return -1;
    }
    fgets(ak, 100, fp);
    int len = strlen(ak);
    ak[len - 1] = '\0';
    printf("ak=%s\n", ak);
    fgets(sk, 100, fp);
    len = strlen(sk);
    sk[len - 1] = '\0';
    printf("sk=%s\n", sk);
    fclose(fp);
    return 0;
}

int createbucket(const Ks3ApiInfo& info) {
    int ret = 0;
    buffer* resp = NULL;
    resp = create_bucket(info.host.c_str(), info.bucket.c_str(), 
            info.access_key.c_str(), info.secret_key.c_str(), NULL, &ret);
    if (ret != 0) {
        return ret;
    }
    if (resp->status_code != 200 && resp->status_code != 409) {
        printf("status code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
        buffer_free(resp);
        return resp->status_code;
    }
    buffer_free(resp);
    return ret;
}


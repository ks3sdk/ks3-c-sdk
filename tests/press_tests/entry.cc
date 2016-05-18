
#include <map>
#include <vector>
#include "press_upload.h"
#include "press_upload_buf.h"
#include "press_download.h"
#include "press_delete.h"

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

Ks3Presser* GetKs3Presser(const string& op);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("[Usage] %s press_upload [dir]\n", argv[0]);
        printf("[Usage] %s press_upload_buf [dir]\n", argv[0]);
        printf("[Usage] %s press_download [dir]\n", argv[0]);
        printf("[Usage] %s press_delete [dir]\n", argv[0]);
        return 0;
    }
    string op = argv[1];
    string src_dir = argv[2];
    printf("entry src_dir=%s, op=%s\n",
            src_dir.c_str(), op.c_str());
    /*
    char* id = "S1guCl0KF/oA285zzEDK";
    char* key = "DGSTgVMQ08EepL3CanUoatVV9en7mB856ljbNEaK";
    char* host   = "kss.ksyun.com"; //hz host
    char* bucket = "c-bucket1";
    char* headers = "x-kss-callbackurl:http://10.4.2.38:19090/";
    */
    Ks3ApiInfo ks3_api_info;
    ks3_api_info.host = "kss.ksyun.com";
    ks3_api_info.bucket = "c-bucket1";
    ks3_api_info.access_key = "S1guCl0KF/qxO4CElPY/";
    ks3_api_info.secret_key = "b7zBDxv9ohTPc0tgc8Hpp89i7I0FDnkyQY4mYY6I";

    int count = 1;
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
    }
    return NULL;
}

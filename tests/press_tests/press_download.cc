
#include "press_download.h"
#include "pandora/time_util.h"
#include "api.h"

namespace ks3_c_sdk
{
namespace test
{

using pandora::TimeUtil;

void Ks3Downloader::HandleFile(const string& local_file,
        const string& object_key, int32_t size) {
    //printf("will upload file=%s, obj_key=%s\n",
    //        localfile.c_str(), object_key.c_str());
    char save_file[100] = { '\0' };
    sprintf(save_file, "./tmp_file_%d", seq_);
    int curl_err;
    int64_t t1 = TimeUtil::GetTime();
    buffer* resp = download_file_object(ks3_api_info_.host.c_str(),
            ks3_api_info_.bucket.c_str(), object_key.c_str(),
            save_file, ks3_api_info_.access_key.c_str(),
            ks3_api_info_.secret_key.c_str(), NULL, &curl_err);
    int64_t t2 = TimeUtil::GetTime();
    if (curl_err != 0) {
        printf("[FAIL]seq=%d, network failure, file=%s, error=%d, ut=%ld us\n",
                seq_, local_file.c_str(), curl_err, (t2 - t1));
        return;
    }
    if (resp->status_code != 200) {
        printf("[FAIL]seq=%d, file=%s, status code=%d, status msg=%s, ut=%ld us\n",
                seq_, local_file.c_str(), resp->status_code, resp->status_msg, (t2 - t1));
        return;
    }
    printf("[OK], seq=%d, file=%s, size=%d, ut=%ld us\n", seq_,
            local_file.c_str(), size, (t2 - t1));
    buffer_free(resp);
}

}  // end of test
}  // end of ks3_c_sdk


#include "press_download.h"
#include "pandora/time_util.h"
#include "api.h"

namespace ks3_c_sdk
{
namespace test
{

using pandora::TimeUtil;

void Ks3Downloader::HandleFile(const string& local_file,
        const string& object_key, int32_t size,
        const string& relative_path) {
    //printf("will download file=%s, obj_key=%s\n",
    //        localfile.c_str(), object_key.c_str());
    int curl_err;
    int64_t t1 = TimeUtil::GetTime();
    char save_file[100] = { '\0' };
    sprintf(save_file, "./%s_save_%d", relative_path.c_str(), seq_);
    buffer* resp = download_file_object(ks3_api_info_.host.c_str(),
            ks3_api_info_.bucket.c_str(), object_key.c_str(),
            save_file, ks3_api_info_.access_key.c_str(),
            ks3_api_info_.secret_key.c_str(), NULL, NULL, &curl_err);
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
    printf("[OK], seq=%d, file=%s, save_file=%s, size=%d, ut=%ld us\n", seq_,
            local_file.c_str(), save_file, size, (t2 - t1));
    buffer_free(resp);
}

}  // end of test
}  // end of ks3_c_sdk

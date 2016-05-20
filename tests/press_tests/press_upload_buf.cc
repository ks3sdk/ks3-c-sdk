
#include "press_upload_buf.h"
#include "pandora/time_util.h"
#include "api.h"

namespace ks3_c_sdk
{
namespace test
{

using pandora::TimeUtil;

void Ks3BufUploader::HandleFile(const string& localfile,
        const string& object_key, int32_t size,
        const string& relative_path) {
    //printf("will upload file=%s, obj_key=%s\n",
    //        localfile.c_str(), object_key.c_str());
    int curl_err;
    int64_t t1 = TimeUtil::GetTime();
    ///////
	FILE* stream = NULL;
	if ((stream = fopen(localfile.c_str(), "r"))	== NULL) {
		printf("[ERROR] seq=%d, cannot open file [%s]\n",
		        seq_, localfile.c_str());
		return;
	}
	fseek(stream, 0, SEEK_END);
	int flen = ftell(stream);

	char* buf = (char*) malloc (flen);
	fseek(stream, 0, SEEK_SET);
	size_t rlen = fread(buf, 1, flen, stream);
	printf("[INFO] seq=%d, file=%s, flen=%d, rlen=%d\n",
	        seq_, localfile.c_str(), flen, rlen);
	fclose(stream);

    buffer* resp = upload_object(ks3_api_info_.host.c_str(),
            ks3_api_info_.bucket.c_str(), object_key.c_str(),
            buf, flen, ks3_api_info_.access_key.c_str(),
            ks3_api_info_.secret_key.c_str(), NULL, NULL, &curl_err);
    int64_t t2 = TimeUtil::GetTime();
    if (curl_err != 0) {
        printf("[FAIL]seq=%d, network failure, file=%s, error=%d, ut=%ld us\n",
                seq_, localfile.c_str(), curl_err, (t2 - t1));
        return;
    }
    if (resp->status_code != 200) {
        printf("[FAIL]seq=%d, file=%s, status code=%d, status msg=%s, ut=%ld us\n",
                seq_, localfile.c_str(), resp->status_code, resp->status_msg, (t2 - t1));
        return;
    }
    printf("[OK], seq=%d, file=%s, size=%d, ut=%ld us\n", seq_,
            localfile.c_str(), size, (t2 - t1));
    buffer_free(resp);
    free(buf);
    buf = NULL;
    /*
	///////

	resp = upload_object(host, "c-bucket1",
		"win32/buf_object_file2", buf, 20480702, id, key, query_args, NULL, &error);
	if (error != 0) {
		printf("curl err=%d\n", error);
	} else {
		printf("status code=%d\n", resp->status_code);
		printf("status msg=%s\n", resp->status_msg);
		if (resp->body != NULL) {
			printf("error msg=%s\n", resp->body);
		}
	}
	free(buf);
	buffer_free(resp);
	////////
    buffer* resp = upload_object(ks3_api_info_.host.c_str(),
            ks3_api_info_.bucket.c_str(), object_key.c_str(),
            localfile.c_str(), ks3_api_info_.access_key.c_str(),
            ks3_api_info_.secret_key.c_str(), NULL, NULL, &curl_err);
    int64_t t2 = TimeUtil::GetTime();
    if (curl_err != 0) {
        printf("[FAIL]seq=%d, network failure, file=%s, error=%d, ut=%ld us\n",
                seq_, localfile.c_str(), curl_err, (t2 - t1));
        return;
    }
    if (resp->status_code != 200) {
        printf("[FAIL]seq=%d, file=%s, status code=%d, status msg=%s, ut=%ld us\n",
                seq_, localfile.c_str(), resp->status_code, resp->status_msg, (t2 - t1));
        return;
    }
    printf("[OK], seq=%d, file=%s, size=%d, ut=%ld us\n", seq_,
            localfile.c_str(), size, (t2 - t1));
    buffer_free(resp);
    */
}

}  // end of test
}  // end of ks3_c_sdk

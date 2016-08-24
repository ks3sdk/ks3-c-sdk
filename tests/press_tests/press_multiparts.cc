#include "press_multiparts.h"
#include <assert.h> 
#include <math.h>
#include "pandora/time_util.h"
#include "api.h"
#include "md5.h"

namespace ks3_c_sdk
{
namespace test
{

using pandora::TimeUtil;

static int b64_encode(const unsigned char* in_buf, int buf_len, char* out) {
    const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const char suffix = '=';
    char *beg = out;

    int rang = 3 * (buf_len / 3);
    int i = 0;
    for (i = 0; i < rang; i += 3) {
        *out++ = table[in_buf[i] >> 2];
        *out++ = table[((in_buf[i] & 3) << 4) + (in_buf[i + 1] >> 4)];
        *out++ = table[((in_buf[i + 1] & 0x0f) << 2) + (in_buf[i + 2] >> 6)];
        *out++ = table[in_buf[i + 2] & 0x3f];
    }
    if (rang < buf_len) {
        *out++ = table[in_buf[rang] >> 2];
        if (rang + 1 == buf_len) {
            *out++ = table[(in_buf[rang] & 0x03) << 4];
            *out++ = suffix;
            *out++ = suffix;
        }
        else {
            *out++ = table[((in_buf[rang] & 0x03) << 4) + (in_buf[rang + 1] >> 4)];
            *out++ = table[in_buf[rang + 1] & 0x0f << 2];
            *out++ = suffix;
        }
        *out = 0;
    }
    return out - beg;
}

static int b64_decode(const char* in_str, unsigned char *out_buf) {
    int tmp_len = 0;
    int len = strlen(in_str);
    int i,j;
    for (i = 0; i < len; i += 4) {
        int nn[4] = { 0 };

        for (j = 0; j < 4; ++j) {
            if (in_str[i + j] >= 'A' && in_str[i + j] <= 'Z')
                nn[j] = in_str[i + j] - 'A';
            else if (in_str[i + j] >= 'a' && in_str[i + j] <= 'z')
                nn[j] = in_str[i + j] - 'a' + 26;
            else if (in_str[i + j] >= '0' && in_str[i + j] <= '9')
                nn[j] = in_str[i + j] - '0' + 52;
            else if (in_str[i + j] == '+')
                nn[j] = 62;
            else if (in_str[i + j] == '/')
                nn[j] = 63;
            else
                nn[i] = 64;
        }

        out_buf[tmp_len++] = (nn[0] << 2) + (nn[1] >> 4);
        if (nn[2] == 64) {
            out_buf[tmp_len++] = nn[1] << 4;
        }
        else {
            out_buf[tmp_len++] = (nn[1] << 4) + (nn[2] >> 2);

            if (nn[3] == 64) {
                out_buf[tmp_len++] = (nn[2] & 0x03) << 6;
            }
            else {
                out_buf[tmp_len++] = ((nn[2] & 0x03) << 6) + nn[3];
            }
        }
    }
    //*out_len = tmp_len;
    return tmp_len;
}

static int compute_file_md5b64(const char *file_path, char *base64_buf) {
    const int  MD5_SIZE = 16;
    const int  READ_DATA_SIZE = 1024;

    int i;    
    int ret;
    unsigned char data[READ_DATA_SIZE];
    unsigned char md5_value[MD5_SIZE];
    MD5_CTX md5;
    FILE* fd;

    fd = fopen(file_path, "r");
    if (NULL == fd) {
        perror("open");
        return -1;
    }
    // init md5
    MD5Init(&md5);
    while (1) {
        ret = fread(data, READ_DATA_SIZE, 1, fd);
        if (1 != ret) {
            perror("read");
            return -1;
        }
        MD5Update(&md5, data, ret);
        if (0 == ret || ret < READ_DATA_SIZE) {
            break;
        }
    }
    fclose(fd);

    MD5Final(&md5, md5_value);
    b64_encode(md5_value, MD5_SIZE, base64_buf);

    return 0;
}

static int compute_buf_md5b64 (unsigned char* buf, int buf_len, char* base64_buf) {
    const int  MD5_SIZE = 16;

    int i;
    unsigned char md5_value[MD5_SIZE];

    MD5_CTX md5;
    // init md5
    MD5Init(&md5);
    MD5Update(&md5, buf, buf_len);
    MD5Final(&md5, md5_value);
    b64_encode(md5_value, MD5_SIZE, base64_buf);

    return 0;
}

static long get_file_size(const char* filename) {
    FILE * fp = NULL;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("[ERROR] open file=%s failed\n", filename);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fclose(fp);
    return len;
}

static long read_file(const char* filename, char* buf, long offset, long len) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("[ERROR] open file=%s failed\n", filename);
        return -1;
    }
    fseek(fp, offset, SEEK_SET);
    long rlen = fread(buf, 1, len, fp);
    if (rlen != len) {
        printf("[ERROR] read file=%s failed: offset=%ld, len=%ld, but read len is : %ld\n", filename, offset, len, rlen);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return rlen;
}


int Ks3Worker::init(const Ks3ApiInfo & info,  const string &filename, 
        const string & object_key, int seq) {
    ks3Info_ = info;
    filename_ = filename;
    object_key_ = object_key;
    seq_ = seq;
    
    int part_num = 0;
    int ret = parser_file(filename.c_str(), part_num);

    buffer * resp = NULL;
    int result = 0;
    string query_str;
    string header_str;
    query_str = "uploads";

    resp = init_multipart_upload(info.host.c_str(), info.bucket.c_str(), object_key.c_str(), 
            info.access_key.c_str(), info.secret_key.c_str(), query_str.c_str(), NULL, &result);
    if (200 != resp->status_code) { 
        printf("init_multipart_upload failed: seq=%d status = %ld\n", seq, resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->header);
        return result;
    }

    char *oid_beg_ptr = strstr(resp->body, "<UploadId>");
    if (oid_beg_ptr) {
        oid_beg_ptr += strlen("<UploadId>");
        char *oid_end_ptr = strstr(oid_beg_ptr, "</UploadId>");
        if (oid_end_ptr) {
            upload_id_.append(oid_beg_ptr, oid_end_ptr - oid_beg_ptr);
        }
    }

    double thread_num = log2((double)part_num);

    if (thread_num <= 1) 
        thread_num_ = 1;
    else if (thread_num >= 10)
        thread_num_ = 10;
    else 
        thread_num_ = thread_num;


    thread_ = new (std::nothrow) CThread[thread_num_];
    assert(thread_ != NULL);

    buf_ = new (std::nothrow) char*[thread_num_];
    assert(buf_ != NULL);
    for ( int i = 0; i < thread_num_; ++i) {
        buf_[i] = new (std::nothrow) char[max_part_size_];
        assert(buf_[i] != NULL);
    }
    return 0;
}

int Ks3Worker::parser_file(const char* filename, int &part_num) {
    const long FIRST_MIN_PART_SIZE = (5 * 1024 * 1024); // 5MB
    const long SECOND_MIN_PART_SIZE = (100 * 1024);     // 100KB
    const long MAX_PART_NUM = 10000;
    
    long file_len = 0;
    int part_size = 0;
    part_num = 0;
    part_size = 0;
    int count = 0;
    long remain_len;

    file_len = get_file_size(filename);
    if (file_len > FIRST_MIN_PART_SIZE * MAX_PART_NUM) 
        part_size = file_len / MAX_PART_NUM;
    else if (file_len >= FIRST_MIN_PART_SIZE && file_len <= FIRST_MIN_PART_SIZE * MAX_PART_NUM)
        part_size = FIRST_MIN_PART_SIZE;
    else if (file_len >= SECOND_MIN_PART_SIZE && file_len < FIRST_MIN_PART_SIZE)
        part_size = SECOND_MIN_PART_SIZE;
    else 
        part_size = file_len;

    max_part_size_ = part_size;
    part_num = file_len / part_size + (file_len % part_size ? 1 : 0);

    remain_len = file_len;
    while (remain_len > 0) {
        count++;
        if (remain_len < part_size)
            part_size = remain_len;
        part_info *info = new part_info(file_len - remain_len, part_size, count);
        assert(info);
        upload_arr_[0].push_back(info);
        remain_len -= part_size;
    }
    assert(part_num == count);
    assert(part_num == upload_arr_[0].size());
    return part_num;
}

void Ks3Worker::Run(CThread *thread, void *arg) {
    part_info *info = NULL;
    char *buf_ptr = *(char **)arg;
    
    char header_str[1024];
    char query_str[1024];
    long read_len = 0;
    char  base64_buf[32];
    int ret_code = 0;
    buffer* resp = NULL;
    int retry_num = 0;
    const int max_retry_num = 3;

    while(1) {
        thread_mutex_[0].Lock();
        if (!upload_arr_[0].empty()) {
            info = upload_arr_[0].front();
            upload_arr_[0].pop_front();
        } else {
            thread_mutex_[0].Unlock();
            break;
        }
        thread_mutex_[0].Unlock();
        retry_num = 0;
RETRY_TAG:        
        read_len = read_file(filename_.c_str(), buf_ptr, info->offset, info->size);
        assert(read_len == info->size);
        
        compute_buf_md5b64((unsigned char*)buf_ptr, info->size, base64_buf);
        snprintf(query_str, 1024, "partNumber=%d&uploadId=%s", info->part_num, upload_id_.c_str());
        snprintf(header_str, 1024, "Content-Md5: %s", base64_buf);
        resp = upload_part(ks3Info_.host.c_str(), ks3Info_.bucket.c_str(), object_key_.c_str(), 
                ks3Info_.access_key.c_str(), ks3Info_.secret_key.c_str(), 
                upload_id_.c_str(), info->part_num, buf_ptr, info->size, NULL, header_str, &ret_code);
        if (ret_code != 0) {
            retry_num++;
            printf("[WARN] %s:%d %s %s %s upload_part failed! ret_code=%d\n",
                __FUNCTION__, __LINE__, object_key_.c_str(), query_str, header_str, ret_code);
            if ( retry_num <= max_retry_num)
                goto RETRY_TAG;
            else {
                printf("[ERROR] %s:%d %s %s %s upload_part failed! ret_code=%d\n",
                    __FUNCTION__, __LINE__, object_key_.c_str(), query_str, header_str, ret_code);
                break;
            }
        }

        if (resp->status_code != 200) {
            printf("test upload_part: seq=%d, partnum=%d\n", seq_, info->part_num);
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);
            printf("[ERROR] %s:%d %s %s upload_part failed!\n",
                __FUNCTION__, __LINE__, query_str, header_str);
            break;
        }
        char *etag_ptr = strstr(resp->header, "ETag: \"");
        if (etag_ptr) {
            etag_ptr += strlen("ETag: \"");
            strncpy(info->etag, etag_ptr, ETAG_LEN);
        }
        
        printf("[OK] seq=%d %s:%d %s %s upload_part OK!\n", 
            seq_, __FUNCTION__, __LINE__, query_str, header_str);
        buffer_free(resp);

        thread_mutex_[1].Lock();
        upload_arr_[1].push_back(info);
        thread_mutex_[1].Unlock();
    }
}

int Ks3Worker::Finish() {
    part_info *info = NULL;
    buffer *resp = NULL;
    string query_str;
    string complete_str;
    int error = 0;
    char tmp[1024];

    Join();
    //upload_arr_[1].sort();

    query_str = "uploadId=";
    query_str += upload_id_;

    complete_str = "<CompleteMultipartUpload>\n";
    while (!upload_arr_[1].empty()) {
        info = upload_arr_[1].front();
        assert(info);
        upload_arr_[1].pop_front();
        snprintf(tmp, 1024, "<Part><PartNumber>%d</PartNumber><ETag>\"%.*s\"</ETag></Part>\n", 
                    info->part_num, ETAG_LEN, info->etag);
        complete_str += tmp; 
        delete info;
        info = NULL;
    }
    complete_str += "</CompleteMultipartUpload>";
    
    resp = complete_multipart_upload(ks3Info_.host.c_str(), ks3Info_.bucket.c_str(), 
            object_key_.c_str(), ks3Info_.access_key.c_str(), ks3Info_.secret_key.c_str(), 
            upload_id_.c_str(), complete_str.c_str(), complete_str.length(), 
            NULL, NULL, &error);
    if (resp->status_code != 200) {
        printf("test complete_multipart_upload: seq=%d\n", seq_);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);

        buffer_free(resp);

        resp = abort_multipart_upload(ks3Info_.host.c_str(), ks3Info_.bucket.c_str(),
                object_key_.c_str(), ks3Info_.access_key.c_str(), 
                ks3Info_.secret_key.c_str(), upload_id_.c_str(),
                query_str.c_str(), NULL, &error);
        if (resp->status_code != 204) {
            printf("test abort_multipart_upload: seq=%d\n", seq_);
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);
        } else 
            printf ("test abort_multipart_upload: %s\n", query_str.c_str());
    }
    buffer_free(resp);

    return 0;
}

void Ks3Multiparts::HandleFile(const string& local_file,
        const string& object_key, int32_t size,
        const string& relative_path) {
    int curl_err;
    int64_t t1 = TimeUtil::GetTime();
    Ks3Worker work;
    int ret = work.init(ks3_api_info_, local_file, object_key, seq_);
    if (ret != 0) {
        printf("[ERROR], seq=%d, file=%s, size=%d\n", seq_, local_file.c_str(), size);
        return;
    }

    ret = work.Start();
    int64_t t2 = TimeUtil::GetTime();
    ret = work.Finish();
    int64_t t3 = TimeUtil::GetTime();
    
    printf("[OK], seq=%d, file=%s, size=%d, \n"
            "upload_ut=%ld us, complete_ut=%ld us\n", seq_,
            local_file.c_str(), size, (t2 - t1), (t3 - t2));
}

}  // end of test
}  // end of ks3_c_sdk

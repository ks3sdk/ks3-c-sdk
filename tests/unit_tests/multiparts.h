#ifndef _MULTIPARTS_H_
#define _MULTIPARTS_H_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "CUnit/Basic.h"
#include "api.h"
#include "md5.h"

#define ETAG_LEN (32)

typedef struct part_result_node {
    int id;
    char etag[ETAG_LEN];
}part_result_node;

static struct tm* cp_localtime(time_t local_sec, struct tm* local_tm, int time_zone) {
    const char days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};  

    time_t now_sec = local_sec;

    local_tm->tm_gmtoff = time_zone * 3600;
    local_tm->tm_zone = NULL;

    now_sec += local_tm->tm_gmtoff;

    local_tm->tm_wday = ((now_sec / 86400 + 4) % 7);

    local_tm->tm_isdst = 0;

    local_tm->tm_sec = now_sec % 60;
    now_sec /= 60;
    local_tm->tm_min = now_sec % 60;
    now_sec /= 60;

    int four_year_hour_num = (int)(now_sec / 35064); // (365*4 + 1) * 24

    local_tm->tm_year = (four_year_hour_num << 2) + 70;
    now_sec %= 35064;

    for(;;)
    {
        int one_year_hour_num = 8760; // 365 * 24
        if((local_tm->tm_year & 3) == 0)
            one_year_hour_num += 24;
        if(now_sec < one_year_hour_num)
            break;
        local_tm->tm_year++;
        now_sec -= one_year_hour_num;
    }

    local_tm->tm_hour = now_sec % 24;
    now_sec /= 24;

    local_tm->tm_yday = (int)now_sec;

    now_sec++;
    if((local_tm->tm_year & 3) == 0)
    {
        if(now_sec > 60)
            now_sec--;
        else 
        {
            if(now_sec == 60)
            {
                local_tm->tm_mon = 1;
                local_tm->tm_mday = 29;
                return local_tm;
            }
        }
    }

    for(local_tm->tm_mon = 0; days[local_tm->tm_mon] < now_sec; local_tm->tm_mon++)
        now_sec -= days[local_tm->tm_mon];

    local_tm->tm_mday = (int)now_sec;

    return local_tm;
}

static const char* now_str(char *buf, int buf_size) {
    struct tm tm_buf;
    cp_localtime(time(NULL), &tm_buf, 8);

    snprintf(buf, buf_size, "%d-%d-%d %d:%d:%d",
            tm_buf.tm_year + 1900, tm_buf.tm_mon + 1, tm_buf.tm_mday,
            tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec);

    return buf;
}

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

static int b64_decode(const char* in_str, unsigned char *out_buf/*, int *out_len*/) {
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

static int compute_buf_md5b64 (char* buf, int buf_len, char* base64_buf) {
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

static int64_t get_file_size(const char* filename) {
    FILE * fp = NULL;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("[ERROR] open file=%s failed\n", filename);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int64_t len = ftell(fp);
    fclose(fp);
    return len;
}

static int64_t read_file(const char* filename, char* buf, int64_t offset, int64_t len) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("[ERROR] open file=%s failed\n", filename);
        return -1;
    }
    fseek(fp, offset, SEEK_SET);
    int64_t rlen = fread(buf, 1, len, fp);
    if (rlen != len) {
        printf("[ERROR] read file=%s failed: offset=%ld, len=%ld,"
                " but read len is : %ld\n", filename, offset, len, rlen);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return rlen;
}

static int multiparts_upload(const char* host, const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key, const char* filename, const char *uploadid,
    part_result_node** result_arr, int *result_num, int *err, int retry_num) {

    const int64_t FIRST_MIN_PART_SIZE = (5 * 1024 * 1024); // 5MB
    const int64_t SECOND_MIN_PART_SIZE = (100 * 1024);     // 100KB

    int error = 0;
    int ret = 0;
    char query_str[1024];
    char header_str[1024];
    buffer *resp = NULL;

    int64_t file_len = 0; 
    int64_t part_size = 0;
    int64_t part_num = 0;

    file_len = get_file_size(filename);
    if (file_len > FIRST_MIN_PART_SIZE * 10000) 
        part_size = file_len / 10000;
    else if (file_len >= FIRST_MIN_PART_SIZE && file_len <= FIRST_MIN_PART_SIZE * 10000)
        part_size = FIRST_MIN_PART_SIZE;
    else if (file_len >= SECOND_MIN_PART_SIZE && file_len < FIRST_MIN_PART_SIZE)
        part_size = SECOND_MIN_PART_SIZE;
    else 
        part_size = file_len;

    int64_t buf_size = part_size;
    char* buf = (char *)malloc(part_size);
    if (!buf) {
        printf("[ERROR] %s:%d alloc memory size:%ld failed! please review code.",
                __FUNCTION__, __LINE__, part_size);
        CU_ASSERT(0);
        return -1;
    }

    part_num = file_len / part_size + (file_len % part_size ? 1 : 0);
    int64_t alloc_size = sizeof(part_result_node) * part_num;
    part_result_node *part_result_arr = (part_result_node *)malloc(alloc_size);
    if (!part_result_arr) {
        free(buf);
        printf("[ERROR] %s:%d alloc memory size:%ld failed! please review code.",
                __FUNCTION__, __LINE__, alloc_size);
        CU_ASSERT(0);
        return -2;
    }

    unsigned char md5_value[16];
    char  base64_buf[32];
    int count = 0;
    int retry_cnt = 0;
    int64_t read_length = 0;
    int64_t remain_len = file_len;

    while (remain_len > 0) {
        retry_cnt = 0;
        count++;
        if (part_size > remain_len)
            part_size = remain_len;

RETRY_ONCE:
        read_length = read_file(filename, buf, file_len - remain_len, part_size);
        CU_ASSERT(read_length == part_size);

        compute_buf_md5b64(buf, part_size, base64_buf);
        snprintf(query_str, 1024, "partNumber=%d&uploadId=%s", count, uploadid);
        snprintf(header_str, 1024, "Content-Md5: %s", base64_buf);
        resp = upload_part(host, bucket, object_key, access_key, secret_key,
                uploadid, count, buf, part_size, NULL, header_str, &error);
        if (resp->status_code != 200) {
            printf("test upload_part:\n");
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);

            if (retry_cnt < retry_num) {
                printf("[WARNNING] %s:%d %s %s upload_part failed, will be retry!\n",
                        __FUNCTION__, __LINE__, query_str, header_str);
                retry_cnt++;
                goto RETRY_ONCE;
            }
            printf("[ERROR] %s:%d %s upload_part failed!\n",
                    __FUNCTION__, __LINE__, header_str);
            ret++;
        }
        char *etag_ptr = strstr(resp->header, "ETag: \"");
        if (etag_ptr) {
            etag_ptr += strlen("ETag: \"");
            strncpy(part_result_arr[count - 1].etag, etag_ptr, ETAG_LEN);
        }
        else {
            printf("[ERROR] %s:%d response no etag\n", __FUNCTION__, __LINE__);
            CU_ASSERT(0);
        }
        printf("[OK] %s:%d %s %s upload_part OK!\n",
                __FUNCTION__, __LINE__, query_str, header_str);
        part_result_arr[count - 1].id = count;
        CU_ASSERT(error == 0);
        CU_ASSERT(200 == resp->status_code);
        buffer_free(resp);
        remain_len -= part_size;
    }
    *err = error;
    *result_num = count;
    *result_arr = part_result_arr;
    if (buf)
        free(buf);
    buf = NULL;

    return ret;
}


#endif // !_MULTIPARTS_H_


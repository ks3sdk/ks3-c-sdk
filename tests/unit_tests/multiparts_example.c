
#include <stdio.h>
#include <string.h>
#include "api.h"
#include "./load_key.h"

//#define COMPLETA

//const char* host = "kss.ksyun.com";
//const char* host = "ks3-cn-beijing-internal.ksyun.com";
//const char* host = "ks3-cn-beijing-internal.ksyun.com";
const char* bucket = "bucket-test-for-multiparts02";

const int   first_min_part_size = 5 * 1024 * 1024;
const int   second_min_part_size = 100 * 1024;

int get_file_size(const char* filename) {
    FILE * fp = NULL;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("[ERROR] open file=%s failed\n", filename);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fclose(fp);
    return len;
}

int read_file(const char* filename, char* buf, int offset, int len) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("[ERROR] open file=%s failed\n", filename);
        return -1;
    }
    fseek(fp, offset, SEEK_SET);
    int rlen = fread(buf, 1, len, fp);
    if (rlen != len) {
        printf("[ERROR] read file=%s failed: offset=%d, len=%d, but read len is : %d\n", filename, offset, len, rlen);
        fclose(fp);
        return -1;
    }
    
    fclose(fp);
    return rlen;
}
void TEST_LIST_PARTS(void) {
    int error;
    const char* filename = "./lib/libcunit.a";
    
    buffer *resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char objectid_str[128] = {0};

    strcpy(query_str, "uploads");
    strcpy(object_key, "test_multiparts_object_04");

    // 1. init_multipart
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, query_str, NULL, &error);
    if (200 != resp->status_code) {
        printf("create_bucket:\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
        buffer_free(resp);
        return;
    }
    
    char *oid_beg_ptr = strstr(resp->body, "<UploadId>");
    if (oid_beg_ptr) {
        oid_beg_ptr += strlen("<UploadId>");
        char *oid_end_ptr = strstr(oid_beg_ptr, "</UploadId>");
        if (oid_end_ptr) {
            strncpy(objectid_str, oid_beg_ptr, oid_end_ptr - oid_beg_ptr);
            objectid_str[oid_end_ptr - oid_beg_ptr] = 0;
        }
    }
    buffer_free(resp);

    // 2. upload part
    int file_len = get_file_size(filename);
    int part_size = 0;
    int part_num = 0;
    if (file_len > first_min_part_size * 10000) {
        part_size = file_len / 10000;
    } else if (file_len >= first_min_part_size && file_len <= first_min_part_size * 10000) {
        part_size = first_min_part_size;
    } else if (file_len >= second_min_part_size && file_len < first_min_part_size) {
        part_size = second_min_part_size;
    } else {
        part_size = file_len;
    }
    
    int buf_size = part_size;
    char* buf = (char *) malloc (part_size);
    if (!buf) {
        printf ("[ERROR] alloc memory failed! please review code.");
        return ;
    }

    int count = 1;
    int remain_len = file_len;
    while (remain_len > 0) {
        if (part_size > remain_len)
            part_size = remain_len;
        read_file(filename, buf, file_len - remain_len, part_size);
        snprintf(query_str, 1024, "partNumber=%d&uploadId=%s", count, objectid_str);
        resp = upload_part(host, bucket, object_key, ak, sk, buf, part_size, 
                query_str, NULL, &error);
        if (resp->status_code != 200) {
            printf("test upload_part:\n");
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);
            return;
        }
        buffer_free(resp);
        count++;
        remain_len -= part_size;
    }
    // 3. list_multipart_uploads
    snprintf(query_str, 1024, "uploads");
    resp = list_multipart_uploads(host, bucket, ak, sk, query_str, NULL, &error);
    if (resp->status_code != 200) {
        printf("test list_multipart_uploads:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        return;
    }
    buffer_free(resp);

    // 4. list_parts
    snprintf(query_str, 1024, "uploadId=%s", objectid_str);
    resp = list_parts(host, bucket, object_key, ak, sk, query_str, NULL, &error);
    if (resp->status_code != 200) {
        printf("test list_parts:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        return;
    }
#if defined(COMPLETA)
    // 5. complete_multipart_upload
    int dat_len = 0;
    int dat_size = buf_size;
    int print_len = 0;
    char *dat = buf;
    
    print_len = snprintf(dat + dat_len, dat_size - dat_len, "<CompleteMultipartUpload>\n");
    dat_len += print_len;

    char *content = resp->body;
    char *par_ptr = strstr(content, "<ns2:Part>");

    while (par_ptr) {
        char *beg = strstr(par_ptr, "<PartNumber>");
        char *end = strstr(par_ptr, "</ETag>");
        end += strlen("</ETag>");
        print_len = snprintf(dat + dat_len, dat_size - dat_len, "<Part>\n%.*s\n</Part>\n", end - beg, beg);
        dat_len += print_len;
        content = end;
        par_ptr = strstr(content, "<ns2:Part>");
    }
    print_len = snprintf(dat + dat_len, dat_size - dat_len, "</CompleteMultipartUpload>\n");
    dat_len += print_len;
    buffer_free(resp);

    snprintf(query_str, 1024, "uploadId=%s", objectid_str);
    snprintf(header_str, 1024, "Content-Length: %d", dat_len);
    snprintf(header_str, 1024, "Content-Type: application/xml");
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk, dat, dat_len, query_str, NULL, &error);
    if (resp->status_code != 200) {
        printf("test complete_multipart_upload:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        //return;
    }

#else
    buffer_free(resp);
    
    // 6. abort_multipart_upload
    snprintf(query_str, 1024, "uploadId=%s", objectid_str);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk, query_str, NULL, &error);
    if (resp->status_code != 204) {
        printf("test abort_multipart_upload:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        return;
    }
    buffer_free(resp);
#endif

    free(buf);
}
/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 * */
int main() {
    ks3_global_init();
    int ret = load_ak_sk();
    if (ret != 0) {
        printf("[ERROR] load ak, sk failed\n");
        return ret;
    }

    //ret = CreateBucket(host, bucket);
    //if (ret != 0) {
    //    printf("[ERROR] create bucket failed\n");
    //    return ret;
    //}

    
    TEST_LIST_PARTS();

    //ret = DeleteBucket(host, bucket);
    //if (ret != 0) {
    //    printf("[ERROR] delete bucket failed\n");
    //}
    ks3_global_destroy();
    return 0;
}

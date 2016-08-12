
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "CUnit/Basic.h"
#include "api.h"
#include "md5.h"
#include "load_key.h"

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 * */
int init_suite1(void) {
    return 0;
}

#define MD5_SIZE        16                                                                                        
#define MD5_STR_LEN     (MD5_SIZE * 2)
#define READ_DATA_SIZE 1024
int compute_file_md5(const char *file_path, char *md5_str) {
    int i;
    int fd;
    int ret;
    unsigned char data[READ_DATA_SIZE];
    unsigned char md5_value[MD5_SIZE];
    MD5_CTX md5;

    fd = open(file_path, O_RDONLY);
    if (-1 == fd) {
        perror("open");
        return -1;
    }

    // init md5
    MD5Init(&md5);

    while (1) {
        ret = read(fd, data, READ_DATA_SIZE);
        if (-1 == ret) {
            perror("read");
            return -1;
        }

        MD5Update(&md5, data, ret);

        if (0 == ret || ret < READ_DATA_SIZE) {
            break;
        }
    }

    close(fd);

    MD5Final(&md5, md5_value);

    for (i = 0; i < MD5_SIZE; i++) {
        snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
    }
    md5_str[MD5_STR_LEN] = '\0'; // add end

    return 0;
}

int compute_buf_md5(char* buf, int buf_len, char* md5_str) {
    int i; 
    unsigned char md5_value[MD5_SIZE];

    MD5_CTX md5;
    // init md5
    MD5Init(&md5);
    MD5Update(&md5, buf, buf_len);
    MD5Final(&md5, md5_value);

    for (i = 0; i < MD5_SIZE; i++) {
        snprintf(md5_str + i*2, 2 + 1, "%02x", md5_value[i]);
    }
    md5_str[MD5_STR_LEN] = '\0'; // add end
}

/* The suite cleanup function.
 *  * Closes the temporary file used by the tests.
 *   * Returns zero on success, non-zero otherwise.
 *    */
int clean_suite1(void) {
    return 0;
}

int file_len(const char* filename) {
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

int read_file(const char* filename, char* buf) {
    FILE* fp = NULL;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("[ERROR] open file=%s failed\n", filename);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int filelen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int rlen = fread(buf, 1, filelen, fp);
    fclose(fp);
    return rlen;
}

int pread_file(const char* filename, char* buf, int offset, int buf_len) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    int ret = pread(fd, buf, buf_len, offset);
    if (ret != buf_len) {
        return -2;
    }
    close(fd);
    return ret;
}


//const char* host = "kss.ksyun.com";
//const char* host = "ks3-cn-beijing.ksyun.com";
//const char* host = "ks3-cn-beijing.ksyun.com";
const char* bucket = "bucket-test-for-download-object";

void TEST_DOWNLOAD_OBJECT(void) {
    int error;
    buffer* resp = NULL;

    const char* obj_key = "unit_test_dir/download_obj1_longlonglonglong::::xxxx";
    const char* filename = "./lib/libcunit.a";

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    int filelen = file_len(filename);
    char* buf = (char *) malloc (filelen);
    memset(buf, '\0', filelen);
    int rlen = read_file(filename, buf);
    CU_ASSERT(rlen == filelen);
    resp = upload_object(host, bucket, obj_key, buf, rlen,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    free(buf);
    buffer_free(resp);

    // download object
    char download_buf_md5_str[MD5_STR_LEN + 1];
    resp = download_object(host, bucket, obj_key,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);

    compute_buf_md5(resp->content, resp->content_length, download_buf_md5_str);
    char file_md5_str[MD5_STR_LEN + 1];
    compute_file_md5(filename, file_md5_str);
    int ret = strncmp(file_md5_str, download_buf_md5_str, MD5_STR_LEN + 1);
    CU_ASSERT(0 == ret);
    if (ret != 0) {
        printf("file md5=%s\n", file_md5_str);
        printf("buf md5=%s\n", download_buf_md5_str);
    }
    printf("\nheaders=%s\n", resp->header);
    printf("body=%s\n", resp->body);
    printf("content->used=%lu\n", resp->content_used);
    printf("content->size=%lu\n", resp->content_size);
    printf("content->length=%ld\n", resp->content_length);
    buffer_free(resp);

    // delete object
    resp = delete_object(host, bucket, obj_key,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_DOWNLOAD_OBJECT_NOT_EXIST(void) {
    int error;
    buffer* resp = NULL;

    const char* obj_key = "unit_test_dir/download_obj2_longlonglonglong::::not_exist";

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    // download object not exist
    resp = download_object(host, bucket, obj_key,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (error != 0) {
        printf("curl error=%d\n", error);
    } else if (404 != resp->status_code) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    printf("\nheaders=%s\n", resp->header);
    printf("body=%s\n", resp->body);
    printf("content->used=%lu\n", resp->content_used);
    printf("content->size=%lu\n", resp->content_size);
    printf("content->length=%ld\n", resp->content_length);
    buffer_free(resp);

    resp = delete_object(host, bucket, obj_key,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_DOWNLOAD_OBJECT_PARTIAL(void) {
    int ret;
    int error;
    buffer* resp = NULL;

    const char* obj_key = "unit_test_dir/download_obj3_longlonglonglong::::xxxx";
    const char* filename = "./lib/libcunit.a";
    const char* headers = "Range: bytes=4-203";

    // delete
    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    // upload 
    int filelen = file_len(filename);
    char* buf = (char *) malloc (filelen);
    memset(buf, '\0', filelen);
    int rlen = read_file(filename, buf);
    CU_ASSERT(rlen == filelen);
    resp = upload_object(host, bucket, obj_key, buf, rlen,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    free(buf);
    buffer_free(resp);

    // download object
    char download_buf_md5_str[MD5_STR_LEN + 1];
    resp = download_object(host, bucket, obj_key,
            ak, sk, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(206 == resp->status_code);
    compute_buf_md5(resp->content, resp->content_length, download_buf_md5_str);
    // compute file md5
    int buf_len = 200;
    char* buf_data = (char*) malloc (buf_len);
    memset(buf_data, '0', buf_len);
    char file_md5_str[MD5_STR_LEN + 1];
    ret = pread_file(filename, buf_data, 4, 200);
    CU_ASSERT(ret == buf_len);
    compute_buf_md5(buf_data, buf_len, file_md5_str);
    ret = strncmp(file_md5_str, download_buf_md5_str, MD5_STR_LEN + 1);
    CU_ASSERT(0 == ret);
    if (ret != 0) {
        printf("file md5=%s\n", file_md5_str);
        printf("buf md5=%s\n", download_buf_md5_str);
    }
    printf("\nheaders=%s\n", resp->header);
    printf("body=%s\n", resp->body);
    free(buf_data);
    buffer_free(resp);

    // delete object
    resp = delete_object(host, bucket, obj_key,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
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
    ret = CreateBucket(host, bucket);
    if (ret != 0) {
        printf("[ERROR] create bucket failed\n");
        return ret;
    }
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        ks3_global_destroy();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (CU_add_test(pSuite, "test download object\n",
                TEST_DOWNLOAD_OBJECT) == NULL ||
            CU_add_test(pSuite, "test download object not exist\n",
                TEST_DOWNLOAD_OBJECT_NOT_EXIST) == NULL ||
            CU_add_test(pSuite, "test download object partial\n",
                TEST_DOWNLOAD_OBJECT_PARTIAL) == NULL) {
        CU_cleanup_registry();
        ks3_global_destroy();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();


    ret = DeleteBucket(host, bucket);
    if (ret != 0) {
        printf("[ERROR] delete bucket failed\n");
    }
    ks3_global_destroy();
    return CU_get_error();
}

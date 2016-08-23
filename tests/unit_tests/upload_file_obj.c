
#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "api.h"
#include "./load_key.h"

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 * */
int init_suite1(void) {
    return 0;
}

/* The suite cleanup function.
 *  * Closes the temporary file used by the tests.
 *   * Returns zero on success, non-zero otherwise.
 *    */
int clean_suite1(void) {
    return 0;
}

//const char* host = "kss.ksyun.com";
//const char* host = "ks3-cn-beijing.ksyun.com";
//const char* host = "ks3-cn-beijing.ksyun.com";
const char* bucket = "bucket-test-for-upload-file-object";

void TEST_UPLOAD_FILE_OBJECT(void) {
    int error;
    buffer* resp = NULL;

    const char* obj_key = "unit_test_dir/upload_file_obj1_longlonglonglong::::xxxx";
    const char* filename = "./lib/libcunit.a";

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

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

void TEST_UPLOAD_FILE_OBJECT_WITH_FILE_NOT_EXIST(void) {
    int error;
    buffer* resp = NULL;

    const char* obj_key = "unit_test_dir/upload_file_obj1_with_file_not_exist";
    const char* filename = "./lib/libcunit.a.notexist";

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(-1 == resp->status_code);
    buffer_free(resp);

    resp = delete_object(host, bucket, obj_key,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_UPLOAD_FILE_OBJECT_EXIST(void) {
    int error;
    buffer* resp = NULL;

    const char* obj_key = "unit_test_dir/upload_file_obj1_longlonglonglong::::exist";
    const char* filename = "./example.c";

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

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

void TEST_UPLOAD_FILE_OBJECT_WITH_CALLBACK(void) {
    int error;
    buffer* resp = NULL;

    const char* obj_key = "unit_test_dir/upload_file_obj1_longlonglonglong::::callback";
    const char* filename = "./example.c";
    const char* headers = "x-kss-callbackurl:http://10.4.2.38:19092/";

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

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

void TEST_UPLOAD_FILE_OBJECT_WITH_HEADERS(void) {
    int error;
    buffer* resp = NULL;

    const char* obj_key = "unit_test_dir/upload_file_obj1_longlonglonglong::::header:callback:content-type";
    const char* filename = "./example.c";
    const char* headers = "Content-Type:text\nx-kss-callbackurl:http://10.4.2.38:19092/";

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

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

void TEST_UPLOAD_FILE_OBJECT_WITH_SPACE_HEADERS(void) {
    int error;
    buffer* resp = NULL;

    const char* obj_key = "unit_test_dir/upload_file_obj1_longlonglonglong::::header:callback:content-type";
    const char* filename = "./example.c";
    const char* headers = "Content-Type :text\nx-kss-callbackurl:http://10.4.2.38:19092/";

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    headers = "Content-Type: text\nx-kss-callbackurl:http://10.4.2.38:19092/";
    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    headers = "Content-Type : text\nx-kss-callbackurl:http://10.4.2.38:19092/";
    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

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

void TEST_UPLOAD_FILE_OBJECT_WITH_ACL(void) {
    int error;
    buffer* resp = NULL;

    const char* obj_key = "unit_test_dir/upload_file_obj1_longlonglonglong::::header:callback:content-type";
    const char* filename = "./example.c";
    const char* headers = "x-kss-acl:public-read\nx-kss-callbackurl:http://10.4.2.38:19092/";

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

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
    if (CU_add_test(pSuite, "test upload file object with file not exist",
                TEST_UPLOAD_FILE_OBJECT_WITH_FILE_NOT_EXIST) == NULL ||
            CU_add_test(pSuite, "test upload file object\n",
                TEST_UPLOAD_FILE_OBJECT) == NULL ||
            CU_add_test(pSuite, "test upload file object exist\n",
                TEST_UPLOAD_FILE_OBJECT_EXIST) == NULL ||
            CU_add_test(pSuite, "test upload file object with callback\n",
                TEST_UPLOAD_FILE_OBJECT_WITH_CALLBACK) == NULL ||
            CU_add_test(pSuite, "test upload file object with headers\n",
                TEST_UPLOAD_FILE_OBJECT_WITH_HEADERS) == NULL ||
            CU_add_test(pSuite, "test upload file object with space headers\n",
                TEST_UPLOAD_FILE_OBJECT_WITH_SPACE_HEADERS) == NULL ||
            CU_add_test(pSuite, "test upload file object with acl\n",
                TEST_UPLOAD_FILE_OBJECT_WITH_ACL) == NULL) {
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

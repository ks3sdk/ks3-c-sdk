
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
const char* bucket = "bucket-test-for-list-object";

void TEST_LIST_OBJECT_BLANK_AK(void) {
    int error;
    buffer* resp = NULL;

    resp = list_bucket_objects(host, bucket, NULL, sk, NULL, &error);
    CU_ASSERT(NULL == resp);
    CU_ASSERT(-1 == error);
}

void TEST_LIST_OBJECT_BLANK_SK(void) {
    int error;
    buffer* resp = NULL;

    resp = list_bucket_objects(host, bucket, ak, NULL, NULL, &error);
    CU_ASSERT(NULL == resp);
    CU_ASSERT(-1 == error);
}

void TEST_LIST_OBJECT_BUCKET_EMPTY(void) {
    int error;
    buffer* resp = NULL;

    resp = list_bucket_objects(host, bucket, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (200 != resp->status_code) {
        printf("\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_LIST_OBJECT_BUCKET_NOT_EMPTY(void) {
    int error;
    buffer* resp = NULL;

    // upload file first
    const char* object_key = "object1-for-list";
    const char* filename = "./lib/libcunit.a";
    resp = upload_file_object(host, bucket, object_key,
            filename, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    resp = list_bucket_objects(host, bucket, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (200 != resp->status_code) {
        printf("\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    }
    buffer_free(resp);

    resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_LIST_OBJECT_BUCKET_NOT_EXIST(void) {
    int error;
    buffer* resp = NULL;

    const char* bucket_not_exist = "bucket-not-exist";
    resp = delete_bucket(host, bucket_not_exist, ak, sk, NULL, &error);
    CU_ASSERT(error == 0);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = list_bucket_objects(host, bucket_not_exist, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    buffer_free(resp);
}

void TEST_LIST_OBJECT_AK_INVALID() {
    int error;
    buffer* resp = NULL;

    const char* access_key = "S1guCl0KF/oA2xxxxxxxxxx";
    resp = list_bucket_objects(host, bucket, access_key, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code); // forbidden
    buffer_free(resp);
}

void TEST_LIST_OBJECT_SK_INVALID() {
    int error;
    buffer* resp = NULL;

    const char* secret_key = "DGSTgVMQ08EepL3CanUoaxxxxxxxxxxxxxxxxxxxxxxx";
    resp = list_bucket_objects(host, bucket, ak, secret_key, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
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
    if (CU_add_test(pSuite, "test list object with blank ak\n",
                TEST_LIST_OBJECT_BLANK_AK) == NULL
          || CU_add_test(pSuite, "test list object with blank sk\n",
                TEST_LIST_OBJECT_BLANK_SK) == NULL
          || CU_add_test(pSuite, "test list object with bucket empty\n",
              TEST_LIST_OBJECT_BUCKET_EMPTY) == NULL
          || CU_add_test(pSuite, "test list object with bucket not empty",
              TEST_LIST_OBJECT_BUCKET_NOT_EMPTY) == NULL
          || CU_add_test(pSuite, "test list object with invalid ak\n",
              TEST_LIST_OBJECT_AK_INVALID) == NULL
          || CU_add_test(pSuite, "test list object with invalid sk\n",
              TEST_LIST_OBJECT_SK_INVALID) == NULL
          || CU_add_test(pSuite, "test list object with bucket not exist\n",
              TEST_LIST_OBJECT_BUCKET_NOT_EXIST) == NULL) {
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

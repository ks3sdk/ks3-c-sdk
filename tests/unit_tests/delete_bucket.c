
#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "api.h"
#include "./load_key.h"

//const char* host = "ks3-cn-beijing-internal.ksyun.com";
//const char* host = "ks3-cn-beijing-internal.ksyun.com";

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

void TEST_DELETE_BUCKET(void) {
    int error;
    buffer* resp = NULL;

    const char* bucket = "bucket-test-for-delete1";

    resp = create_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    // delete bucket finally
    resp = delete_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_DELETE_BUCKET_NOT_EXIST(void) {
    int error;
    buffer* resp = NULL;

    const char* bucket = "bucket-test-for-delete2";

    resp = delete_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    buffer_free(resp);
}

void TEST_DELETE_BUCKET_NOT_EMPTY(void) {
    int error;
    buffer* resp = NULL;

    const char* bucket = "bucket-test-for-delete3";
    // create bucket first
    resp = create_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("create bucket status code = %ld\n", resp->status_code);
        printf("create bucket status msg = %s\n", resp->status_msg);
        printf("create bucket error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    sleep(3);

    // upload object
    const char* obj_key = "unit_test_dir/upload_obj1_longlonglonglong::::xxxx";
    const char* filename = "./lib/libcunit.a";
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

    // delete bucket not empty
    resp = delete_bucket(host, bucket, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(409 == resp->status_code);
    if (resp->status_code != 409) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    sleep(3);

    // delete object
    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // delete bucket finally
    resp = delete_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    sleep(3);
}

void TEST_DELETE_BUCKET_WITH_BLANK_NAME(void) {
    int error;
    buffer* resp = NULL;

    const char* bucket = NULL;

    resp = delete_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code);
    if (405 != resp->status_code) {
        printf("\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
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
    if (CU_add_test(pSuite, "test delete bucket\n",
                TEST_DELETE_BUCKET) == NULL ||
            CU_add_test(pSuite, "test delete bucket not exist\n",
                TEST_DELETE_BUCKET_NOT_EXIST) == NULL ||
            CU_add_test(pSuite, "test delete bucket with blank name\n",
                TEST_DELETE_BUCKET_WITH_BLANK_NAME) == NULL ||
            CU_add_test(pSuite, "test delete bucket not empty\n",
                TEST_DELETE_BUCKET_NOT_EMPTY) == NULL) {
        CU_cleanup_registry();
        ks3_global_destroy();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    ks3_global_destroy();
    return CU_get_error();
}

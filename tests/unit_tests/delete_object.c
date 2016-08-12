
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
//const char* bucket = "c-bucket1";
const char* bucket = "bucket-test-for-delete-object";

void TEST_DEL_OBJ() {
    int error;
    buffer* resp = NULL;
    const char* obj_key = "unit-test-delete-obj1";
    const char* filename = "./lib/libcunit.a";

    // upload obj first
    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    // delete obj then
    resp = delete_object(host, bucket, obj_key,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_DEL_OBJ_NOT_EXIST(void) {
    buffer* resp = NULL;
    const char* obj_key = "not-exist-obj";
    int error;

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    buffer_free(resp);
}

void TEST_DEL_OBJ_WITH_BLANK_BUCKET_NAME(void) {
    buffer* resp = NULL;
    const char* s_bucket = NULL;
    const char* obj_key = "not-exist-obj";
    int error;

    resp = delete_object(host, s_bucket, obj_key,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code); // Request method 'DELETE' not supporte''
    if (resp->status_code != 405) {
        printf("\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
    }
    buffer_free(resp);
}

void TEST_DEL_OBJ_WITH_BLANK_OBJECT_NAME(void) {
    buffer* resp = NULL;
    int error;

    const char* obj_key = "unit-test-delete-object";
    const char* filename = "./lib/libcunit.a";

    resp = upload_file_object(host, bucket, obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    const char* blank_obj_key = NULL;
    resp = delete_object(host, bucket, blank_obj_key,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(409 == resp->status_code);
    if (resp->status_code != 409) {
        printf("\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
    }
    buffer_free(resp);

    resp = delete_object(host, bucket, obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_DEL_OBJ_WITH_BLANK_BUCKET_NAME_AND_OBJECT_NAME(void) {
    buffer* resp = NULL;
    const char* s_bucket = NULL;
    const char* obj_key = NULL;
    int error;
    resp = delete_object(host, s_bucket, obj_key,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code); // Request method 'DELETE' not supporte''
    if (resp->status_code != 405) {
        printf("\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
    }
    buffer_free(resp);
}

void TEST_DEL_OBJ_WITH_BLANK_HOST_NAME_AND_BUCKET_NAME_AND_OBJECT_NAME(void) {
    buffer* resp = NULL;
    const char* s_host = NULL;
    const char* s_bucket = NULL;
    const char* obj_key = NULL;
    int error;
    resp = delete_object(s_host, s_bucket, obj_key,
            ak, sk, NULL, &error);
    CU_ASSERT(3 == error);
    if (error != 3) {
        printf("\ncurl error=%d\n", error);
    }
    buffer_free(resp);
}

void TEST_DEL_OBJ_WITH_BLANK_HOST_NAME_AND_BUCKET_NAME(void) {
    buffer* resp = NULL;
    const char* s_host = NULL;
    const char* s_bucket = NULL;
    const char* obj_key = "unit-test-delete-object/blank-host-bucket-name";
    int error;
    resp = delete_object(s_host, s_bucket, obj_key,
            ak, sk, NULL, &error);
    CU_ASSERT(3 == error);
    if (error != 3) {
        printf("\ncurl error=%d\n", error);
    }
    buffer_free(resp);
}

void TEST_DEL_OBJ_WITH_BLANK_HOST_NAME_AND_OBJECT_NAME(void) {
    buffer* resp = NULL;
    const char* s_host = NULL;
    const char* obj_key = NULL;
    int error;
    resp = delete_object(s_host, bucket, obj_key,
            ak, sk, NULL, &error);
    CU_ASSERT(3 == error);
    if (error != 3) {
        printf("\ncurl error=%d\n", error);
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
    if (CU_add_test(pSuite, "test delete object\n",
                TEST_DEL_OBJ) == NULL ||
            CU_add_test(pSuite, "test delete_object not exist\n",
                TEST_DEL_OBJ_NOT_EXIST) == NULL ||
            CU_add_test(pSuite, "test delete_object with blank bucket name\n",
                TEST_DEL_OBJ_WITH_BLANK_BUCKET_NAME) == NULL ||
            CU_add_test(pSuite, "test delete_object with blank object name\n",
                TEST_DEL_OBJ_WITH_BLANK_OBJECT_NAME) == NULL ||
            CU_add_test(pSuite, "test delete_object with blank bucket name and object name\n",
                TEST_DEL_OBJ_WITH_BLANK_BUCKET_NAME_AND_OBJECT_NAME) == NULL ||
            CU_add_test(pSuite, "test delete_object with blank host name, bucket name and obj name\n",
                TEST_DEL_OBJ_WITH_BLANK_HOST_NAME_AND_BUCKET_NAME_AND_OBJECT_NAME) == NULL ||
            CU_add_test(pSuite, "test delete_object with blank host name and bucket name\n",
                TEST_DEL_OBJ_WITH_BLANK_HOST_NAME_AND_BUCKET_NAME) == NULL ||
            CU_add_test(pSuite, "test delete_object with blank host name and bucket name\n",
                TEST_DEL_OBJ_WITH_BLANK_HOST_NAME_AND_OBJECT_NAME) == NULL) {
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

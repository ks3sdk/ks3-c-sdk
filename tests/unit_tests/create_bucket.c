
#include <stdio.h>
#include <stdlib.h>
#include "api.h"
#include "CUnit/Basic.h"
#include "./load_key.h"


int init_suite1() {
    return 0;
}

int clean_suite1() {
    return 0;
}

//const char* host = "kss.ksyun.com";
//const char* host = "ks3-cn-beijing.ksyun.com";
//const char* host = "ks3-cn-beijing-internal.ksyun.com";
//const char* host = "ks3-cn-beijing-internal.ksyun.com";
//const char* host = "ks3-cn-shanghai.ksyun.com";

void TEST_CREATE_BUCKET(void) {
    int error;
    buffer* resp = NULL;

    const char* bucket = "bucket-test-for-create1";

    resp = create_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (200 != resp->status_code) {
        printf("status code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    }
    buffer_free(resp);

    // delete bucket finally
    resp = delete_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_CREATE_BUCKET_EXIST(void) {
    int error;
    buffer* resp = NULL;

    const char* bucket = "bucket-test-for-create2";

    resp = create_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    resp = create_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(409 == resp->status_code);
    buffer_free(resp);

    // delete bucket finally
    resp = delete_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_CREATE_BUCKET_WITH_BLANK_BUCKET_NAME(void) {
    int error;
    buffer* resp = NULL;

    const char* bucket = NULL;

    resp = create_bucket(host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code);
    if (405 != resp->status_code) {
        printf("\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
    }
    buffer_free(resp);
}

void TEST_CREATE_BUCKET_WITH_BLANK_HOST_NAME(void) {
    int error;
    buffer* resp = NULL;

    const char* s_host = NULL;
    const char* bucket = "bucket-test-for-create3";

    resp = create_bucket(s_host, bucket,
            ak, sk, NULL, &error);
    CU_ASSERT(3 == error);
    if (error != 3) {
        printf("\ncurl error=%d\n", error);
    }
    buffer_free(resp);
}

void TEST_CREATE_BUCKET_WITH_BLANK_HOST_NAME_AND_BUCKET_NAME(void) {
    int error;
    buffer* resp = NULL;

    const char* s_host = NULL;
    const char* bucket = NULL;

    resp = create_bucket(s_host, bucket,
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
    CU_pSuite pSuite = NULL;

    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        ks3_global_destroy();
        return CU_get_error();
    }

    if (CU_add_test(pSuite, "test create bucket\n",
                TEST_CREATE_BUCKET) == NULL ||
            CU_add_test(pSuite, "test create bucket exist\n",
                TEST_CREATE_BUCKET_EXIST) == NULL ||
            CU_add_test(pSuite, "test create bucket with blank bucket name\n",
                TEST_CREATE_BUCKET_WITH_BLANK_BUCKET_NAME) == NULL ||
            CU_add_test(pSuite, "test create bucket with blank host name\n",
                TEST_CREATE_BUCKET_WITH_BLANK_HOST_NAME) == NULL ||
            CU_add_test(pSuite, "test create bucket with blank host name and bucket name\n",
                TEST_CREATE_BUCKET_WITH_BLANK_HOST_NAME_AND_BUCKET_NAME) == NULL) {
        CU_cleanup_registry();
        ks3_global_destroy();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    ks3_global_destroy();
    return CU_get_error();
}

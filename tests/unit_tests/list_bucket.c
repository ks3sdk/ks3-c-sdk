
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

void TEST_LIST_ALL_BUCKETS(void) {
    int error;
    buffer* resp = NULL;

    resp = list_all_bucket(host, ak, sk, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (200 != resp->status_code) {
        printf("\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    }

    buffer_free(resp);
}

void TEST_LIST_BUCKET_AK_INVALID() {
    int error;
    buffer* resp = NULL;
    const char* access_key = "S1guCl0KF/oA2xxxxxxxxxx";
    resp = list_all_bucket(host, access_key, sk, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    printf("status msg=%s\n", resp->status_msg);
    printf("error msg=%s\n", resp->body);
    buffer_free(resp);
}

void TEST_LIST_BUCKET_SK_INVALID() {
    int error;
    buffer* resp = NULL;

    const char* secret_key = "DGSTgVMQ08EepL3CanUoaxxxxxxxxxxxxxxxxxxxxxxx";
    resp = list_all_bucket(host, ak, secret_key, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    printf("status code=%ld\n", resp->status_code);
    printf("status msg=%s\n", resp->status_msg);
    printf("error msg=%s\n", resp->body);
    buffer_free(resp);
}

void TEST_LIST_BUCKET_WITH_BLANK_HOST(void) {
    int error;
    buffer* resp = NULL;

    const char* s_host = NULL;
    resp = list_all_bucket(s_host, ak, sk, &error);
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
    if (CU_add_test(pSuite, "test list all bucket\n",
                TEST_LIST_ALL_BUCKETS) == NULL ||
            CU_add_test(pSuite, "test list bucket with invalid ak\n",
              TEST_LIST_BUCKET_AK_INVALID) == NULL ||
            CU_add_test(pSuite, "test list bucket with invalid sk\n",
                TEST_LIST_BUCKET_SK_INVALID) == NULL ||
            CU_add_test(pSuite, "test list bucket with blank host\n",
                TEST_LIST_BUCKET_WITH_BLANK_HOST) == NULL) {
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


#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "api.h"

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

const char* access_key = "S1guCl0KF/oA285zzEDK";
const char* secret_key = "DGSTgVMQ08EepL3CanUoatVV9en7mB856ljbNEaK";
const char* host = "kss.ksyun.com";
const char* bucket = "c-bucket1";

void TEST_DEL_OBJ() {
    int error;
    buffer* resp = NULL;
    const char* obj_key = "unit-test-delete-obj1";
    const char* filename = "./lib/libcunit.a";

    // upload obj first
    resp = upload_file_object(host, bucket, obj_key, filename,
            access_key, secret_key, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    // delete obj then
    resp = delete_object(host, bucket, obj_key,
            access_key, secret_key, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_DEL_OBJ_NOT_EXIST(void) {
    buffer* resp = NULL;
    const char* obj_key = "not-exist-obj";
    int error;
    resp = delete_object(host, bucket, obj_key,
            access_key, secret_key, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    buffer_free(resp);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 * */
int main() {
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (CU_add_test(pSuite, "test delete object\n",
                TEST_DEL_OBJ) == NULL ||
            CU_add_test(pSuite, "test delete_object not exist\n",
                TEST_DEL_OBJ_NOT_EXIST) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

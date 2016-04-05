
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

const char* host = "kss.ksyun.com";

void TEST_UPLOAD_OBJECT(void) {
    int error;
    buffer* resp = NULL;

    const char* access_key = "S1guCl0KF/oA285zzEDK";
    const char* secret_key = "DGSTgVMQ08EepL3CanUoatVV9en7mB856ljbNEaK";
    const char* bucket = "c-bucket1";

    const char* obj_key = "unit_test_dir/upload_obj1_longlonglonglong::::xxxx";
    const char* filename = "./lib/libcunit.a";
    resp = upload_file_object(host, bucket, obj_key, filename,
            access_key, secret_key, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_UPLOAD_OBJECT_EXIST(void) {
    int error;
    buffer* resp = NULL;

    const char* access_key = "S1guCl0KF/oA285zzEDK";
    const char* secret_key = "DGSTgVMQ08EepL3CanUoatVV9en7mB856ljbNEaK";
    const char* bucket = "c-bucket1";

    const char* obj_key = "unit_test_dir/upload_obj1_longlonglonglong::::exist";
    const char* filename = "./example.c";
    resp = upload_file_object(host, bucket, obj_key, filename,
            access_key, secret_key, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = upload_file_object(host, bucket, obj_key, filename,
            access_key, secret_key, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_UPLOAD_OBJECT_WITH_CALLBACK(void) {
    int error;
    buffer* resp = NULL;

    const char* access_key = "S1guCl0KF/oA285zzEDK";
    const char* secret_key = "DGSTgVMQ08EepL3CanUoatVV9en7mB856ljbNEaK";
    const char* bucket = "c-bucket1";

    const char* obj_key = "unit_test_dir/upload_obj1_longlonglonglong::::callback";
    const char* filename = "./example.c";
    const char* headers = "x-kss-callbackurl:http://10.4.2.38:19090/";

    resp = upload_file_object(host, bucket, obj_key, filename,
            access_key, secret_key, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_UPLOAD_OBJECT_WITH_HEADERS(void) {
    int error;
    buffer* resp = NULL;

    const char* access_key = "S1guCl0KF/oA285zzEDK";
    const char* secret_key = "DGSTgVMQ08EepL3CanUoatVV9en7mB856ljbNEaK";
    const char* bucket = "c-bucket1";

    const char* obj_key = "unit_test_dir/upload_obj1_longlonglonglong::::header:callback:content-type";
    const char* filename = "./example.c";
    const char* headers = "Content-Type:text\nx-kss-callbackurl:http://10.4.2.38:19090/";

    resp = upload_file_object(host, bucket, obj_key, filename,
            access_key, secret_key, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_UPLOAD_OBJECT_WITH_SPACE_HEADERS(void) {
    int error;
    buffer* resp = NULL;

    const char* access_key = "S1guCl0KF/oA285zzEDK";
    const char* secret_key = "DGSTgVMQ08EepL3CanUoatVV9en7mB856ljbNEaK";
    const char* bucket = "c-bucket1";

    const char* obj_key = "unit_test_dir/upload_obj1_longlonglonglong::::header:callback:content-type";
    const char* filename = "./example.c";
    const char* headers = "Content-Type :text\nx-kss-callbackurl:http://10.4.2.38:19090/";

    resp = upload_file_object(host, bucket, obj_key, filename,
            access_key, secret_key, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    headers = "Content-Type: text\nx-kss-callbackurl:http://10.4.2.38:19090/";
    resp = upload_file_object(host, bucket, obj_key, filename,
            access_key, secret_key, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    headers = "Content-Type : text\nx-kss-callbackurl:http://10.4.2.38:19090/";
    resp = upload_file_object(host, bucket, obj_key, filename,
            access_key, secret_key, NULL, headers, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
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
    if (CU_add_test(pSuite, "test upload object\n",
                TEST_UPLOAD_OBJECT) == NULL ||
            CU_add_test(pSuite, "test upload object exist\n",
                TEST_UPLOAD_OBJECT_EXIST) == NULL ||
            CU_add_test(pSuite, "test upload object with callback\n",
                TEST_UPLOAD_OBJECT_WITH_CALLBACK) == NULL ||
            CU_add_test(pSuite, "test upload object with headers\n",
                TEST_UPLOAD_OBJECT_WITH_HEADERS) == NULL ||
            CU_add_test(pSuite, "test upload object with space headers\n",
                TEST_UPLOAD_OBJECT_WITH_SPACE_HEADERS) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

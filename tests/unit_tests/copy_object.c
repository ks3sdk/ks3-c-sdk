
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

const char* host = "kss.ksyun.com";

void TEST_COPY_OBJECT_WITH_SAME_BUCKET(void) {
    int error;
    buffer* resp = NULL;

    const char* src_bucket = "c-bucket1";
    const char* src_obj_key = "unit_test_dir/src_object1";

    const char* filename = "./lib/libcunit.a";
    resp = upload_file_object(host, src_bucket, src_obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        buffer_free(resp);
        return;
    }
    buffer_free(resp);

    error = -1;
    const char* dst_bucket = src_bucket;
    const char* dst_obj_key = "unit_test_dir/dst_object1";
    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    printf("status code = %ld\n", resp->status_code);
    printf("status msg = %s\n", resp->status_msg);
    printf("error msg = %s\n", resp->body);
    buffer_free(resp);

    resp = delete_object(host, dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_SAME_BUCKET_AND_OBJ_KEY(void) {
    int error;
    buffer* resp = NULL;

    const char* src_bucket = "c-bucket1";
    const char* src_obj_key = "unit_test_dir/src_object1";

    const char* filename = "./lib/libcunit.a";
    resp = upload_file_object(host, src_bucket, src_obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        buffer_free(resp);
        return;
    }
    buffer_free(resp);

    error = -1;
    const char* dst_bucket = src_bucket;
    const char* dst_obj_key = src_obj_key;
    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_DIFF_BUCKET() {
    int error;
    buffer* resp = NULL;

    const char* src_bucket = "c-bucket1";
    const char* src_obj_key = "unit_test_dir/src_object1";

    // upload first
    const char* filename = "./lib/libcunit.a";
    resp = upload_file_object(host, src_bucket, src_obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        buffer_free(resp);
        return;
    }
    buffer_free(resp);

    // create dst bucket
    const char* dst_bucket = "dst-bucket-1";
    const char* dst_obj_key = src_obj_key;
    resp = create_bucket(host, dst_bucket, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    // copy obj
    error = -1;
    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    printf("status code = %ld\n", resp->status_code);
    printf("status msg = %s\n", resp->status_msg);
    printf("error msg = %s\n", resp->body);
    buffer_free(resp);

    // delete dst obj
    resp = delete_object(host, dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // delete dst bucket
    resp = delete_bucket(host, dst_bucket, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }

    // delete src obj
    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_DIFF_BUCKET_AND_EXIST_KEY() {
    int error;
    buffer* resp = NULL;

    const char* src_bucket = "c-bucket1";
    const char* src_obj_key = "unit_test_dir/src_object1";

    // upload first
    const char* filename = "./lib/libcunit.a";
    resp = upload_file_object(host, src_bucket, src_obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        buffer_free(resp);
        return;
    }
    buffer_free(resp);

    // create dst bucket
    const char* dst_bucket = "dst-bucket-2";
    const char* dst_obj_key = src_obj_key;
    resp = create_bucket(host, dst_bucket, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    // copy obj first time
    error = -1;
    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    printf("status code = %ld\n", resp->status_code);
    printf("status msg = %s\n", resp->status_msg);
    printf("error msg = %s\n", resp->body);
    buffer_free(resp);
    sleep(5);
    // copy obj second time
    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code);
    printf("status code = %ld\n", resp->status_code);
    printf("status msg = %s\n", resp->status_msg);
    printf("error msg = %s\n", resp->body);
    buffer_free(resp);

    // delete dst obj
    resp = delete_object(host, dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // delete dst bucket
    resp = delete_bucket(host, dst_bucket, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }

    // delete src obj
    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
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
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (CU_add_test(pSuite, "test copy object with same bucket\n",
                TEST_COPY_OBJECT_WITH_SAME_BUCKET) == NULL ||
            CU_add_test(pSuite, "test copy objec with same bucket and key \n",
                TEST_COPY_OBJECT_WITH_SAME_BUCKET_AND_OBJ_KEY) == NULL ||
            CU_add_test(pSuite, "test copy objec with diff bucket \n",
                TEST_COPY_OBJECT_WITH_DIFF_BUCKET) == NULL ||
            CU_add_test(pSuite, "test copy objec with diff bucket and exist key\n",
                TEST_COPY_OBJECT_WITH_DIFF_BUCKET_AND_EXIST_KEY) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

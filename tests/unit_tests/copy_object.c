
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

//const char* host = "ks3-cn-beijing-internal.ksyun.com";
//const char* host = "ks3-cn-beijing-internal.ksyun.com";
//const char* host = "ks3-cn-beijing.ksyun.com";
//const char* host = "ks3-cn-beijing.ksyun.com";
const char* src_bucket = "bucket-src-for-copy-object-test";
const char* dst_bucket = "bucket-dst-for-copy-object-test";

void TEST_COPY_OBJECT_WITH_SAME_BUCKET(void) {
    int error;
    buffer* resp = NULL;

    const char* src_obj_key = "unit_test_dir/src_object1";
    const char* filename = "./lib/libcunit.a";

    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, src_bucket, src_obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    const char* l_dst_bucket = src_bucket;
    const char* dst_obj_key = "unit_test_dir/dst_object1";

    resp = delete_object(host, l_dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = copy_object(host, src_bucket, src_obj_key,
            l_dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);

    resp = delete_object(host, l_dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_SAME_BUCKET_AND_SAME_OBJ_KEY(void) {
    int error;
    buffer* resp = NULL;

    const char* src_obj_key = "unit_test_dir/src_object1";
    const char* filename = "./lib/libcunit.a";

    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, src_bucket, src_obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    const char* l_dst_bucket = src_bucket;
    const char* dst_obj_key = src_obj_key;
    resp = copy_object(host, src_bucket, src_obj_key,
            l_dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code);
    if (resp->status_code != 400) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_SAME_BUCKET_AND_DIFF_EXIST_OBJ_KEY(void) {
    int error;
    buffer* resp = NULL;

    const char* src_obj_key = "unit_test_dir/src_object1";
    const char* filename = "./lib/libcunit.a";

    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, src_bucket, src_obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    const char* l_dst_bucket = src_bucket;
    const char* dst_obj_key = "dst_object1";

    resp = delete_object(host, l_dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    // copy first
    resp = copy_object(host, src_bucket, src_obj_key,
            l_dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // copy again
    resp = copy_object(host, src_bucket, src_obj_key,
            l_dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code); // Bad Request
    if (resp->status_code != 400) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);

    resp = delete_object(host, l_dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_DIFF_BUCKET() {
    int error;
    buffer* resp = NULL;

    const char* src_obj_key = "unit_test_dir/src_object1";

    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

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
    }
    buffer_free(resp);

    // copy obj
    const char* dst_obj_key = src_obj_key;

    resp = delete_object(host, dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (200 != resp->status_code) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // delete src and dst obj
    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);

    resp = delete_object(host, dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_DIFF_BUCKET_AND_EXIST_KEY() {
    int error;
    buffer* resp = NULL;

    // upload first
    const char* src_obj_key = "unit_test_dir/src_object1";
    const char* filename = "./lib/libcunit.a";

    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, src_bucket, src_obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // copy obj first time
    const char* dst_obj_key = "unit-test-dir/dst_obj1";

    resp = delete_object(host, dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (200 != resp->status_code) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // copy obj second time
    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code);
    if (400 != resp->status_code) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // delete dst obj
    resp = delete_object(host, dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);

    // delete src obj
    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_SRC_BUCKET_NOT_EXIST() {
    int error;
    buffer* resp = NULL;

    const char* src_bucket_not_exist = "src-bucket-not-exist";
    const char* src_obj_key = "unit_test_dir/src_object1";

    // delete src bucket
    resp = delete_bucket(host, src_bucket_not_exist, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    buffer_free(resp);

    // copy obj
    const char* dst_obj_key = src_obj_key;
    resp = copy_object(host, src_bucket_not_exist, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (404 != resp->status_code) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_SRC_OBJECT_NOT_EXIST() {
    int error;
    buffer* resp = NULL;

    const char* src_obj_key = "src_object_not_exist";

    // delete src object 
    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    // copy obj
    const char* dst_obj_key = src_obj_key;
    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (404 != resp->status_code) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_DST_BUCKET_NOT_EXIST() {
    int error;
    buffer* resp = NULL;

    const char* src_obj_key = "unit_test_dir/src_object1";
    const char* filename = "./lib/libcunit.a";

    // upload first
    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, src_bucket, src_obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    // copy obj
    const char* dst_bucket_not_exist = "dst-bucket-not-exist";
    const char* dst_obj_key = src_obj_key;

    resp = delete_bucket(host, dst_bucket_not_exist, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    buffer_free(resp);

    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket_not_exist, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (404 != resp->status_code) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // delete src obj
    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_BLANK_DST_BUCKET_NAME() {
    int error;
    buffer* resp = NULL;

    const char* src_obj_key = "unit_test_dir/src_object1";
    const char* filename = "./lib/libcunit.a";

    // upload first
    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code || 404 == resp->status_code);
    buffer_free(resp);

    resp = upload_file_object(host, src_bucket, src_obj_key, filename,
            ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    // copy obj 
    const char* dst_bucket = NULL;
    const char* dst_obj_key = src_obj_key;
    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code);
    if (405 != resp->status_code) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // delete dst obj
    resp = delete_object(host, dst_bucket, dst_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code);
    if (resp->status_code != 405) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // delete dst bucket
    resp = delete_bucket(host, dst_bucket, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code);
    if (resp->status_code != 405) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // delete src obj
    resp = delete_object(host, src_bucket, src_obj_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_BLANK_SRC_BUCKET_NAME() {
    int error;
    buffer* resp = NULL;

    const char* src_bucket_null = NULL;
    const char* src_obj_key = "src_object1";
    const char* dst_obj_key = "dst_object1";

    // copy obj with blank src bucket name
    resp = copy_object(host, src_bucket_null, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code);
    if (400 != resp->status_code) {
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_COPY_OBJECT_WITH_BLANK_SRC_OBJECT_NAME() {
    int error;
    buffer* resp = NULL;

    const char* src_obj_key = NULL;
    const char* dst_obj_key = "dst-object1";

    // copy obj with blank src object name
    resp = copy_object(host, src_bucket, src_obj_key,
            dst_bucket, dst_obj_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code); // invalid argument
    if (400 != resp->status_code) {
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
    ret = CreateBucket(host, src_bucket);
    if (ret != 0) {
        printf("[ERROR] create src bucket failed\n");
        return ret;
    }
    ret = CreateBucket(host, dst_bucket);
    if (ret != 0) {
        printf("[ERROR] create dst bucket failed\n");
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
    if (CU_add_test(pSuite, "test copy object with same bucket\n",
                TEST_COPY_OBJECT_WITH_SAME_BUCKET) == NULL ||
            CU_add_test(pSuite, "test copy object with same bucket and key \n",
                TEST_COPY_OBJECT_WITH_SAME_BUCKET_AND_SAME_OBJ_KEY) == NULL ||
            CU_add_test(pSuite, "test copy object with same bucket and diff exist key \n",
                TEST_COPY_OBJECT_WITH_SAME_BUCKET_AND_DIFF_EXIST_OBJ_KEY) == NULL ||
            CU_add_test(pSuite, "test copy object with diff bucket \n",
                TEST_COPY_OBJECT_WITH_DIFF_BUCKET) == NULL ||
            CU_add_test(pSuite, "test copy object with diff bucket and exist key\n",
                TEST_COPY_OBJECT_WITH_DIFF_BUCKET_AND_EXIST_KEY) == NULL ||
            CU_add_test(pSuite, "test copy object with src bucket not exist\n",
                TEST_COPY_OBJECT_WITH_SRC_BUCKET_NOT_EXIST) == NULL ||
            CU_add_test(pSuite, "test copy object with src object not exist\n",
                TEST_COPY_OBJECT_WITH_SRC_OBJECT_NOT_EXIST) == NULL ||
            CU_add_test(pSuite, "test copy object with dst bucket not exist\n",
                TEST_COPY_OBJECT_WITH_DST_BUCKET_NOT_EXIST) == NULL ||
            CU_add_test(pSuite, "test copy object with blank dst bucket name\n",
                TEST_COPY_OBJECT_WITH_BLANK_DST_BUCKET_NAME) == NULL ||
            CU_add_test(pSuite, "test copy object with blank src bucket name\n",
                TEST_COPY_OBJECT_WITH_BLANK_SRC_BUCKET_NAME) == NULL ||
            CU_add_test(pSuite, "test copy object with blank src object name\n",
                TEST_COPY_OBJECT_WITH_BLANK_SRC_OBJECT_NAME) == NULL) {
        CU_cleanup_registry();
        ks3_global_destroy();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    ret = DeleteBucket(host, src_bucket);
    if (ret != 0) {
        printf("[ERROR] delete src_bucket failed\n");
    }
    ret = DeleteBucket(host, dst_bucket);
    if (ret != 0) {
        printf("[ERROR] delete dst_bucket failed\n");
    }
    ks3_global_destroy();
    return CU_get_error();
}

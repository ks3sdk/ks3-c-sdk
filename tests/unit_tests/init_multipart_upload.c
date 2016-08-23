#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "api.h"
#include "md5.h"
#include "./load_key.h"
#include "multiparts.h"


/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 * */
int init_suite1(void) {
    printf("%s:%d", __FUNCTION__, __LINE__);
    return 0;
}

/* The suite cleanup function.
 *  * Closes the temporary file used by the tests.
 *   * Returns zero on success, non-zero otherwise.
 *    */
int clean_suite1(void) {
    printf("%s:%d", __FUNCTION__, __LINE__);
    return 0;
}


int error;
buffer *resp = NULL;
char object_key[1024];
char query_str[1024];
char header_str[1024];
char objectid_str[128];
char access_key[128];
char secret_key[128];

const char *bucket = "bucket-test-for-init-multipart-upload";

void TEST_INIT_MULTIPART_UPLOAD_ALL_NULL(void) {
    resp = init_multipart_upload(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    return;
}

void TEST_INIT_MULTIPART_UPLOAD_RETURNCODE_NULL(void) {
    
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = init_multipart_upload(host, bucket, object_key, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    
    resp = init_multipart_upload(host, bucket, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = init_multipart_upload(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    return;
}

void TEST_INIT_MULTIPART_UPLOAD_HOST(void) {
    
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = init_multipart_upload(NULL, bucket, object_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);

    resp = init_multipart_upload(NULL, NULL, object_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(3 == error || 6 == error);
    buffer_free(resp);
    
    resp = init_multipart_upload(NULL, NULL, NULL, ak, sk, NULL, NULL, &error);
    CU_ASSERT(3 == error || 6 == error);
    buffer_free(resp);

    resp = init_multipart_upload(NULL, NULL, NULL, NULL, NULL, NULL, NULL, &error); 
    CU_ASSERT(-1 == error);                                                      
    CU_ASSERT(NULL == resp);

    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    //resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    //CU_ASSERT(0 == error);
    //CU_ASSERT(204 == resp->status_code);
    //if (resp->status_code != 204) {
    //    printf("test %s:%d:\n", __FUNCTION__, __LINE__);
    //    printf("status code = %ld\n", resp->status_code);
    //    printf("status msg = %s\n", resp->status_msg);
    //    printf("error msg = %s\n", resp->body);
    //}
    //buffer_free(resp);


    return;
}
void TEST_INIT_MULTIPART_UPLOAD_BUCKET(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = init_multipart_upload(host, NULL, NULL, NULL, NULL, NULL, NULL, &error);
    buffer_free(resp);
    
    resp = init_multipart_upload(host, NULL, NULL, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    resp = init_multipart_upload(host, NULL, object_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = init_multipart_upload(host, bucket, NULL, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // bucket is not exist
    resp = init_multipart_upload(host, "test-c-sdk-bucket", object_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // normal
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    //resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    //CU_ASSERT(0 == error);
    //CU_ASSERT(204 == resp->status_code);
    //if (resp->status_code != 204) {
    //    printf("test %s:%d:\n", __FUNCTION__, __LINE__);
    //    printf("status code = %ld\n", resp->status_code);
    //    printf("status msg = %s\n", resp->status_msg);
    //    printf("error msg = %s\n", resp->body);
    //}
    //buffer_free(resp);
    
    return;
}

void TEST_INIT_MULTIPART_UPLOAD_OBJECT(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);

    resp = init_multipart_upload(host, bucket, NULL, NULL, NULL, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = init_multipart_upload(host, bucket, object_key, NULL, NULL, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    return;
}
void TEST_INIT_MULTIPART_UPLOAD_KEY(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);

    resp = init_multipart_upload(host, bucket, object_key, NULL, NULL, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = init_multipart_upload(host, bucket, object_key, ak, NULL, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = init_multipart_upload(host, bucket, object_key, NULL, sk, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = init_multipart_upload(host, bucket, object_key, "asdfghjkl", "qwertyuiop", NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    return;
}
void TEST_INIT_MULTIPART_UPLOAD_QUERYPARA(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    snprintf(query_str, 1024, "%s", "uploads ");
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    // HTTP/1.1 400 Unknown Version 
    snprintf(query_str, 1024, "%s", " uploads");
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code);
    if (resp->status_code != 400) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    snprintf(query_str, 1024, "%s", "?uploads");
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    

    snprintf(query_str, 1024, "%s", "uploads");
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    return;
}
void TEST_INIT_MULTIPART_UPLOAD_HEADERPARA(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);

    snprintf(header_str, 1024, "%s", "content-language: \ncontent-type: ");
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    snprintf(header_str, 1024, "%s", "Content-Language: en,cn,us,*");
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    snprintf(header_str, 1024, "%s", "");
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    snprintf(header_str, 1024, "%s", "abc: 123\n456: cba");
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    return;
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
    if (NULL == CU_add_test(pSuite, "test init para null\n", TEST_INIT_MULTIPART_UPLOAD_ALL_NULL)
        || NULL == CU_add_test(pSuite, "test init para return code null\n", TEST_INIT_MULTIPART_UPLOAD_RETURNCODE_NULL)
        || NULL == CU_add_test(pSuite, "test init para host\n", TEST_INIT_MULTIPART_UPLOAD_HOST)
        || NULL == CU_add_test(pSuite, "test init para bucket\n", TEST_INIT_MULTIPART_UPLOAD_BUCKET)
        || NULL == CU_add_test(pSuite, "test init para object\n", TEST_INIT_MULTIPART_UPLOAD_OBJECT)
        || NULL == CU_add_test(pSuite, "test init para key\n", TEST_INIT_MULTIPART_UPLOAD_KEY)
        || NULL == CU_add_test(pSuite, "test init para query\n", TEST_INIT_MULTIPART_UPLOAD_QUERYPARA)
        || NULL == CU_add_test(pSuite, "test init para header\n", TEST_INIT_MULTIPART_UPLOAD_HEADERPARA)) {
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


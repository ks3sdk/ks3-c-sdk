#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "api.h"
#include "md5.h"
#include "./load_key.h"
//#include "multiparts.h"


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
char object_key[1024];
char query_str[1024];
char header_str[1024];
char objectid_str[128];
char access_key[128];
char secret_key[128];

const char* bucket = "bucket-test-for-abort-multipart-upload";

void TEST_ABORT_MULTIPART_UPLOAD_ALL_NULL(void) {
    buffer *resp = NULL;
    resp = abort_multipart_upload(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
}

void TEST_ABORT_MULTIPART_UPLOAD_RETURNCODE_NULL(void) {
    buffer *resp = NULL;
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = abort_multipart_upload(host, bucket, object_key, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    
    resp = abort_multipart_upload(host, bucket, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = abort_multipart_upload(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
}

void TEST_ABORT_MULTIPART_UPLOAD_HOST(void) {
    const char* upload_id = "test";
    buffer *resp = NULL;
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = abort_multipart_upload(NULL, bucket, object_key, ak, sk, upload_id, NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);

    resp = abort_multipart_upload(NULL, NULL, object_key, ak, sk, upload_id, NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);
    
    resp = abort_multipart_upload(NULL, NULL, NULL, ak, sk, upload_id, NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);

    resp = abort_multipart_upload(NULL, NULL, NULL, NULL, NULL, upload_id, NULL, NULL, &error); 
    CU_ASSERT(-1 == error);                                                      
    CU_ASSERT(NULL == resp);
}

void TEST_ABORT_MULTIPART_UPLOAD_NORMAL(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    // init first
    buffer* resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }
    char uploadId[128] = { '\0' };
    GetUploadId(resp->body, uploadId);
    CU_ASSERT(strlen(uploadId) > 0);
    buffer_free(resp);                                                                 
    // abort uploadId
    resp = NULL; error = -1;
    char query_args[200] = { '\0' };
    snprintf(query_args, 1024, "uploadId=%s", uploadId);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk, uploadId, query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }
    buffer_free(resp);
    // abort uploadId not exist
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk, uploadId, query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }
    buffer_free(resp);
}

void TEST_ABORT_MULTIPART_UPLOAD_BUCKET(void) {
    const char* upload_id = "test";
    buffer *resp = NULL;
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = abort_multipart_upload(host, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &error);
    CU_ASSERT(NULL == resp);
    buffer_free(resp);
    
    resp = abort_multipart_upload(host, NULL, NULL, ak, sk, upload_id, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code);
    if (resp->status_code != 405) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    resp = abort_multipart_upload(host, NULL, object_key, ak, sk, upload_id, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code);
    if (resp->status_code != 405) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    /*
    // acutally delete bucket
    resp = abort_multipart_upload(host, bucket, NULL, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    */

    // bucket not exist
    resp = abort_multipart_upload(host, "bucket-not-exist", object_key,
            ak, sk, upload_id, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_ABORT_MULTIPART_UPLOAD_OBJECT(void) {
    const char* upload_id = "test";
    buffer *resp = NULL;
    snprintf(object_key, 1024, "%s", __FUNCTION__);

    resp = abort_multipart_upload(host, bucket, NULL, NULL, NULL,
            upload_id, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = abort_multipart_upload(host, bucket, object_key, NULL, NULL,
            upload_id,  NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = abort_multipart_upload(host, bucket, object_key, ak, sk,
            upload_id, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    return;
}
void TEST_ABORT_MULTIPART_UPLOAD_KEY(void) {
    const char* upload_id = "test";
    buffer *resp = NULL;
    snprintf(object_key, 1024, "%s", __FUNCTION__);

    resp = abort_multipart_upload(host, bucket, object_key, NULL,
            NULL, upload_id, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = abort_multipart_upload(host, bucket, object_key, ak, NULL,
            upload_id, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = abort_multipart_upload(host, bucket, object_key, NULL,
            sk, upload_id, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = abort_multipart_upload(host, bucket, object_key, "asdfghjkl", "qwertyuiop",
            upload_id, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk,
            upload_id, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    return;
}

void TEST_ABORT_MULTIPART_UPLOAD_QUERYPARA(void) {
    char uploadId[100] = { '\0' };
    buffer *resp = NULL;
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    GetUploadId(resp->body, uploadId);
    buffer_free(resp);
    // abort
    char query_args[100] = { '\0' };
    snprintf(query_args, 100, " uploadId=%s", uploadId);
    // invalid http request : has space in query_args
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk,
            uploadId, query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code);
    if (resp->status_code != 400) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // abort : query args smaller letter
    snprintf(query_args, 100, "uploadid=%s", uploadId);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk,
            uploadId, query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // abort : query args bigger letter
    snprintf(query_args, 100, "uploadID=%s", uploadId);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk,
            uploadId, query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // abort : normal
    snprintf(query_args, 100, "uploadId=%s", uploadId);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk,
            uploadId, query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
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
    if (NULL == CU_add_test(pSuite, "test abort para null\n",
                TEST_ABORT_MULTIPART_UPLOAD_ALL_NULL)
        || NULL == CU_add_test(pSuite, "test abort para key\n",
            TEST_ABORT_MULTIPART_UPLOAD_NORMAL)
        || NULL == CU_add_test(pSuite, "test abort para return code null\n",
            TEST_ABORT_MULTIPART_UPLOAD_RETURNCODE_NULL)
        || NULL == CU_add_test(pSuite, "test abort para host\n",
            TEST_ABORT_MULTIPART_UPLOAD_HOST)
        || NULL == CU_add_test(pSuite, "test abort para bucket\n",
            TEST_ABORT_MULTIPART_UPLOAD_BUCKET)
        || NULL == CU_add_test(pSuite, "test abort para object\n",
            TEST_ABORT_MULTIPART_UPLOAD_OBJECT)
        || NULL == CU_add_test(pSuite, "test abort para key\n",
            TEST_ABORT_MULTIPART_UPLOAD_KEY)
        || NULL == CU_add_test(pSuite, "test abort para query\n",
            TEST_ABORT_MULTIPART_UPLOAD_QUERYPARA)) {
        CU_cleanup_registry();
        ks3_global_destroy();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    //ret = DeleteBucket(host, bucket);
    //if (ret != 0) {
    //    printf("[ERROR] delete bucket failed\n");
    //}
    
    ks3_global_destroy();
    return CU_get_error();
}


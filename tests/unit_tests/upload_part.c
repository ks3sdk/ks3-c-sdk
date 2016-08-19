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

const char *bucket = "bucket-test-for-upload-part";

void TEST_UPLOAD_PART_ALL_NULL(void) {
    resp = upload_part(NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    return;
}

void TEST_UPLOAD_PART_RETURNCODE_NULL(void) {
    
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = upload_part(host, bucket, object_key, ak, sk, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = upload_part(host, bucket, object_key, NULL, NULL, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    
    resp = upload_part(host, bucket, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = upload_part(NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    return;
}

void TEST_UPLOAD_PART_HOST(void) {
    
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = upload_part(NULL, bucket, object_key, ak, sk, NULL, 0, NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);

    resp = upload_part(NULL, NULL, object_key, ak, sk, NULL, 0, NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);
    
    resp = upload_part(NULL, NULL, NULL, ak, sk, NULL, 0, NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);

    resp = upload_part(NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, &error); 
    CU_ASSERT(-1 == error);                                                      
    CU_ASSERT(NULL == resp);

    // upload null object is allowed
    resp = upload_part(host, bucket, object_key, ak, sk, NULL, 0, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    

    resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    if (resp->status_code != 204) {
        printf("test delete_object:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
    return;
}
void TEST_UPLOAD_PART_BUCKET(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
     
    resp = upload_part(host, NULL, NULL, ak, sk, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code); // HTTP/1.1 405 Request method 'PUT' not supported
    if (resp->status_code != 405) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    resp = upload_part(host, NULL, object_key, ak, sk, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code); // HTTP/1.1 405 Request method 'PUT' not supported
    if (resp->status_code != 405) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // HTTP/1.1 409 Conflict
    // <Code>BucketAlreadyExists</Code>
    // <Message>The requested bucket name is not available. The bucket namespace is shared by all users of the system. Please select a different name and try again.</Message>
    resp = upload_part(host, bucket, NULL, ak, sk, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(409 == resp->status_code); // HTTP/1.1 409 Conflict 
    if (resp->status_code != 409) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // HTTP/1.1 404 Not Found
    // <Code>NoSuchBucket</Code>
    // <Message>The specified bucket does not exist.</Message>
    resp = upload_part(host, "test-c-sdk-bucket", object_key, ak, sk, "123", 3, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // HTTP/1.1 404 Not Found
    // <Code>NotFoundApi</Code>
    // <Message>not found api</Message>
    resp = upload_part(host, "", object_key, ak, sk, "0123456789", 10, NULL, NULL, &error);
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

void TEST_UPLOAD_PART_OBJECT(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    // HTTP/1.1 409 Conflict <Code>BucketAlreadyExists</Code>
    resp = upload_part(host, bucket, NULL, ak, sk, "0123456789", 10,  NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(409 == resp->status_code);
    if (resp->status_code != 409) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // HTTP/1.1 409 Conflict <Code>BucketAlreadyExists</Code>
    resp = upload_part(host, bucket, "", ak, sk, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(409 == resp->status_code);
    if (resp->status_code != 409) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // query_str is null, equal to upload_object
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    if (resp->status_code != 204) {
        printf("test delete_object:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
    return;
}
void TEST_UPLOAD_PART_KEY(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);

    resp = upload_part(host, bucket, object_key, NULL, NULL, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = upload_part(host, bucket, object_key, ak, NULL, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = upload_part(host, bucket, object_key, NULL, sk, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    // Forbidden  <Code>InvalidAccessKeyId</Code>\n    <Message>The KS3 access key Id you provided does not exist in our records.</Message>
    resp = upload_part(host, bucket, object_key, "asdfghjkl", "qwertyuiop", "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // query_str is null, equal to upload_object
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    if (resp->status_code != 204) {
        printf("test delete_object:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
    return;
}
void TEST_UPLOAD_PART_QUERYPARA(void) {
    objectid_str[0] = 0;
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    snprintf(query_str, 1024, "%s", "");
    // query_str no content, equal to upload_object
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", 10, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    if (resp->status_code != 204) {
        printf("test delete_object:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(204 == resp->status_code);
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
        buffer_free(resp);
        return;
    } else {
        char *oid_beg_ptr = strstr(resp->body, "<UploadId>");
        if (oid_beg_ptr) {
            oid_beg_ptr += strlen("<UploadId>");
            char *oid_end_ptr = strstr(oid_beg_ptr, "</UploadId>");
            if (oid_end_ptr) {
                strncpy(objectid_str, oid_beg_ptr, oid_end_ptr - oid_beg_ptr);
                objectid_str[oid_end_ptr - oid_beg_ptr] = 0;
            }
        }
    }
    buffer_free(resp);
    // 404 Not Found
    // <Code>NoSuchUpload</Code>
    // <Message>The specified multipart upload does not exist. 
    // The upload ID might be invalid, or the multipart upload 
    // might have been aborted or completed.</Message>
    snprintf(query_str, 1024, "partNumber=%d&uploadId=%s", 1, "123");
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", 10, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    // HTTP/1.1 400 Bad Request, partNumber is 1 .. 10000
    // <Code>InvalidPartNum</Code><Message>invalid partNum</Message>
    snprintf(query_str, 1024, "partNumber=%d&uploadId=%s", 0, objectid_str);
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", 10, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code);
    if (resp->status_code != 400) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // <Code>InvalidPartNum</Code><Message>invalid partNum</Message>
    snprintf(query_str, 1024, "partNumber=%d&uploadId=%s", 10001, objectid_str);
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", 10, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code);
    if (resp->status_code != 400) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // <Code>NotFoundApi</Code> <Message>not found api</Message>
    snprintf(query_str, 1024, "uploadId=%s", objectid_str);
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", 10, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
     
    snprintf(query_str, 1024, "partNumber=%d&uploadId=%s", 1, objectid_str);
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", 10, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    snprintf(query_str, 1024, "uploadId=%s", objectid_str);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk, query_str, NULL, &error);
    if (resp->status_code != 204) {
        printf("test abort_multipart_upload:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        return;
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
    return;
}

void TEST_UPLOAD_PART_HEADERPARA(void) {
    char md5_buf[32];
    objectid_str[0] = 0;
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    snprintf(query_str, 1024, "%s", "");
    snprintf(header_str, 1024, "%s", "");
    // query_str no content, equal to upload_object
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", 10, query_str, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    if (resp->status_code != 204) {
        printf("test delete_object:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(204 == resp->status_code);
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
        buffer_free(resp);
        return;
    } else {
        char *oid_beg_ptr = strstr(resp->body, "<UploadId>");
        if (oid_beg_ptr) {
            oid_beg_ptr += strlen("<UploadId>");
            char *oid_end_ptr = strstr(oid_beg_ptr, "</UploadId>");
            if (oid_end_ptr) {
                strncpy(objectid_str, oid_beg_ptr, oid_end_ptr - oid_beg_ptr);
                objectid_str[oid_end_ptr - oid_beg_ptr] = 0;
            }
        }
    }
    buffer_free(resp);
    
    snprintf(query_str, 1024, "partNumber=%d&uploadId=%s", 1, objectid_str);

    resp = upload_part(host, bucket, object_key, ak, sk, NULL, 10, query_str, NULL, &error);
    CU_ASSERT(NULL == resp);
    
    resp = upload_part(host, bucket, object_key, ak, sk, "1", 10, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // HTTP/1.1 411 Length Required
    // <Code>MissingContentLength</Code><Message>You must provide the Content-Length HTTP header.</Message>
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", -1, query_str, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(411 == resp->status_code);
    if (resp->status_code != 411) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    // md5 error HTTP/1.1 400 Bad Request
    // <Code>InvalidDigest</Code><Message>The Content-MD5 you specified is not valid.</Message>
    snprintf(header_str, 1024, "Content-MD5: %s", "itisincorrect==");
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", 10, query_str, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code);
    if (resp->status_code != 400) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    // md5 
    compute_buf_md5b64((char*)"0123456789", 10, md5_buf);
    snprintf(header_str, 1024, "Content-MD5: %s", md5_buf);
    resp = upload_part(host, bucket, object_key, ak, sk, "0123456789", 10, query_str, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    
    snprintf(query_str, 1024, "uploadId=%s", objectid_str);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk, query_str, NULL, &error);
    if (resp->status_code != 204) {
        printf("test abort_multipart_upload:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        return;
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);
    
    return;
}

void clean(void) {
    int error;
    buffer *resp = NULL;
    buffer *sub_resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char upload_id[128] = { 0 };
    char sub_object_key[1024];
    char sub_query_str[1024];
    int max_uploads = 200;
    char upload_id_marker[1024];
    char key_marker[1024];
    int has_next = 0;
    char *key_ptr = NULL;
    char *key_end = NULL;
    int   key_len = 0;

    // list_multipart_uploads
    snprintf(query_str, 1024, "uploads&max-uploads=%d", max_uploads);
    do {
        has_next = 0;
        upload_id_marker[0] = 0;
        key_marker[0] = 0;

        resp = list_multipart_uploads(host, bucket, ak, sk, query_str, NULL, &error);
        if (resp->status_code != 200) {
            printf("test list_multipart_uploads:\n");
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);

        }
        CU_ASSERT(error == 0);
        CU_ASSERT(200 == resp->status_code);
        
        char *content = resp->body;
        char *next_key = strstr(content, "<NextKeyMarker>");
        if (next_key) {
            next_key += strlen("<NextKeyMarker>");
            char * end = strstr(next_key, "</NextKeyMarker>");
            if (end) 
                snprintf(key_marker, 1024, "%.*s", end - next_key, next_key);
            
            char * next_upload_id = strstr(content, "<NextUploadIdMarker>");
            if (next_upload_id) {
                next_upload_id += strlen("<NextUploadIdMarker>");

                end = strstr(next_upload_id, "</NextUploadIdMarker>");
                if (end)
                    snprintf(upload_id_marker, 1024, "%.*s", end - next_upload_id, next_upload_id);
            }
        }
        if (upload_id_marker[0] != 0 && key_marker[0] != 0) {
            has_next = 1;
            snprintf(query_str, 1024, "uploads&max-uploads=%d&key-marker=%s&upload-id-marker=%s", max_uploads, key_marker, upload_id_marker);
        }
        key_ptr = strstr(content, "<Key>");
        while(key_ptr) {
            key_ptr += strlen("<Key>");
            key_end = strstr(key_ptr, "</Key>");
            key_len = key_end - key_ptr;
            snprintf(sub_object_key, 1024, "%.*s", key_len, key_ptr);
            
            key_ptr = strstr(key_end, "<UploadId>");
            key_ptr += strlen("<UploadId>");
            key_end = strstr(key_ptr, "</UploadId>");
            key_len = key_end - key_ptr;
            snprintf(sub_query_str, 1024, "uploadId=%.*s", key_len, key_ptr);

            sub_resp = abort_multipart_upload(host, bucket, sub_object_key, ak, sk, sub_query_str, NULL, &error);
            if (sub_resp->status_code != 204) {
                printf("test %s:%d abort_multipart_upload:\n", __FUNCTION__, __LINE__);
                printf("status code = %ld\n", sub_resp->status_code);
                printf("status msg = %s\n", sub_resp->status_msg);
                printf("error msg = %s\n", sub_resp->body);
            }
            CU_ASSERT(error == 0);
            CU_ASSERT(204 == sub_resp->status_code);
            buffer_free(sub_resp);
            
            key_ptr = strstr(key_end, "<Key>");
        }
        
        buffer_free(resp);
    } while (has_next);

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
    if (NULL == CU_add_test(pSuite, "test upload part para null\n", TEST_UPLOAD_PART_ALL_NULL)
        || NULL == CU_add_test(pSuite, "test upload part para return code null\n", TEST_UPLOAD_PART_RETURNCODE_NULL)
        || NULL == CU_add_test(pSuite, "test upload part para host\n", TEST_UPLOAD_PART_HOST)
        || NULL == CU_add_test(pSuite, "test upload part para bucket\n", TEST_UPLOAD_PART_BUCKET)
        || NULL == CU_add_test(pSuite, "test upload part para object\n", TEST_UPLOAD_PART_OBJECT)
        || NULL == CU_add_test(pSuite, "test upload part para key\n", TEST_UPLOAD_PART_KEY)
        || NULL == CU_add_test(pSuite, "test upload part para query\n", TEST_UPLOAD_PART_QUERYPARA)
        || NULL == CU_add_test(pSuite, "test upload part para header\n", TEST_UPLOAD_PART_HEADERPARA)
        || NULL == CU_add_test(pSuite, "clean bucket\n", clean)) {
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


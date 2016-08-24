#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "api.h"
#include "md5.h"
#include "./load_key.h"


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

void GetNextUploadIdMarker(const char* body, char* value) {
    const char* marker_start = "<NextUploadIdMarker>";
    const char* marker_end = "</NextUploadIdMarker>";
    int len = strlen(marker_start);

    char* begin = strstr(body, marker_start);
    if (begin != NULL) {
        char* end = strstr(begin + len, marker_end);
        strncpy(value, begin + len, end - begin - len);
    }
}

void GetTruncated(const char* body, char* value) {
    char* is_truncated_start = "<IsTruncated>";
    char* is_truncated_end = "</IsTruncated>";
    int len = strlen(is_truncated_start);

    char* begin = strstr(body, is_truncated_start);
    if (begin != NULL) {
        char* end = strstr(begin + len, is_truncated_end);
        strncpy(value, begin + len, end - begin - len);
    } else {
        CU_ASSERT(0 == 1);
    }
}


int error;
buffer *resp = NULL;
char object_key[1024];
char query_str[1024];
char header_str[1024];
char uploadid_str[128];

const char *bucket = "bucket-test-for-list-multipart-uploads";

void clean_bucket(void) {
    buffer *sub_resp = NULL;
    char sub_object_key[1024];
    char sub_query_str[1024];
    char sub_header_str[1024];    
    int max_uploads = 200;
    char upload_id_marker[1024];
    char key_marker[1024];
    int has_next = 0;
    char *key_ptr = NULL;
    char *key_end = NULL;
    int   key_len = 0;
   
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
            snprintf(query_str, 1024,
                    "uploads&max-uploads=%d&key-marker=%s&upload-id-marker=%s",
                    max_uploads, key_marker, upload_id_marker);
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
            char upload_id[1024] = { '\0' };
            strncpy(upload_id, key_ptr, key_len);

            sub_resp = abort_multipart_upload(host, bucket, sub_object_key,
                    ak, sk, upload_id, NULL, NULL, &error);
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

void TEST_LIST_MULTIPART_UPLOADS_ALL_NULL(void) {
    resp = list_multipart_uploads(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    
    return;
}

void TEST_LIST_MULTIPART_UPLOADS_RETURNCODE_NULL(void) {
    
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = list_multipart_uploads(host, bucket, object_key, ak, sk, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = list_multipart_uploads(host, bucket, object_key, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    
    resp = list_multipart_uploads(host, bucket, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = list_multipart_uploads(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    
    resp = list_multipart_uploads("", "", "", "", "", "", NULL);
    CU_ASSERT(NULL == resp);
    
    return;
}

void TEST_LIST_MULTIPART_UPLOADS_HOST(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    // init multipart upload
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
    // list multipart upload normally
    char* query_args = "uploads&uplads";
    resp = list_multipart_uploads(host, bucket, ak, sk, query_args, NULL, &error);
    CU_ASSERT(resp != NULL);
    CU_ASSERT(resp->status_code == 200);
    buffer_free(resp);

    // host is blank
    resp = list_multipart_uploads("", bucket, ak, sk, query_args, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);
    
    // host is NULL
    resp = list_multipart_uploads(NULL, bucket, ak, sk, query_args, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);

    // host is invalid
    resp = list_multipart_uploads("ks3-cn-shanghai.ksyun.com", bucket,
            ak, sk, query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(307 == resp->status_code);
    if (resp->status_code != 307) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    resp = list_multipart_uploads("www.kingsoft.com", bucket,
            ak, sk, query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    // abort multipart uplaod finally
    char uploadIdStr[100] = { '\0' };
    snprintf(uploadIdStr, 100, "uploadId=%s", uploadId);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk, uploadId, NULL, NULL, &error);
    CU_ASSERT(resp != NULL);
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

void TEST_LIST_MULTIPART_UPLOADS_BUCKET(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
   
    // init multipart upload first
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
    // list multipart upload normally
    char* query_args = "uploads&uplads";
    resp = list_multipart_uploads(host, bucket, ak, sk, query_args, NULL, &error);
    CU_ASSERT(resp != NULL);
    CU_ASSERT(resp->status_code == 200);
    buffer_free(resp);

    // param bucket is blank
    resp = list_multipart_uploads(host, "", ak, sk, query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    
    // param bucket is NULL
    resp = list_multipart_uploads(host, NULL, ak, sk, query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    // param bucket is not exist
    resp = list_multipart_uploads(host, "not-exist-bucket", ak, sk, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);

    // bucket in hz, but uplaodId in bj
    resp = list_multipart_uploads(host, "c-bucket1", ak, sk, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(307 == resp->status_code);
    if (resp->status_code != 307) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    resp = list_multipart_uploads(host, bucket, ak, sk, query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);

    // abort uploadId finally
    char uploadIdStr[100] = { '\0' };
    snprintf(uploadIdStr, 100, "uploadId=%s", uploadId);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk, uploadId, NULL, NULL, &error);
    CU_ASSERT(resp != NULL);
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

void TEST_LIST_MULTIPART_UPLOADS_KEY(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
   
    // init multipart upload first
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
    // list multipart upload normally
    char* query_args = "uploads&uplads";
    resp = list_multipart_uploads(host, bucket, ak, sk, query_args, NULL, &error);
    CU_ASSERT(resp != NULL);
    CU_ASSERT(resp->status_code == 200);
    buffer_free(resp);
    
    // ak, sk is NULL
    resp = list_multipart_uploads(host, bucket, NULL, NULL, query_args, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = list_multipart_uploads(host, bucket, "", NULL, query_args, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = list_multipart_uploads(host, bucket, NULL, "", query_args, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = list_multipart_uploads(host, bucket, "", "", query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    
    // ak, sk is invalid
    resp = list_multipart_uploads(host, bucket, "sdfasdfasd",
            "7s9d7fasdf9asd89fasdfhsd", query_args, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    
    // abort uploadId finally
    char uploadIdStr[100] = { '\0' };
    snprintf(uploadIdStr, 100, "uploadId=%s", uploadId);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk, uploadId, NULL, NULL, &error);
    CU_ASSERT(resp != NULL);
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

void TEST_LIST_MULTIPART_UPLOADS_QUERYPARA(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    // init multipart upload first
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
    // list multipart upload normally
    char* query_args = "uploads&uplads";
    resp = list_multipart_uploads(host, bucket, ak, sk, query_args, NULL, &error);
    CU_ASSERT(resp != NULL);
    CU_ASSERT(resp->status_code == 200);
    buffer_free(resp);

    // query_args is NULL
    resp = list_multipart_uploads(host, bucket, ak, sk, NULL, NULL, &error);
    CU_ASSERT(resp != NULL);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);
    
    resp = list_multipart_uploads(host, bucket, ak, sk, "", NULL, &error);
    CU_ASSERT(resp != NULL);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    // query_args add big case UPLOADS
    snprintf(query_str, sizeof(query_str), "UPLOADS", uploadid_str);
    resp = list_multipart_uploads(host, bucket, ak, sk, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // abort uploadId finally
    char uploadIdStr[100] = { '\0' };
    snprintf(uploadIdStr, 100, "uploadId=%s", uploadId);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk, uploadId, NULL, NULL, &error);
    CU_ASSERT(resp != NULL);
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

void TEST_LIST_MULTIPART_UPLOADS_HEADERPARA(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    // init multipart upload first
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
    // header is NULL : list multipart upload normally
    resp = list_multipart_uploads(host, bucket, ak, sk, NULL, NULL, &error);
    CU_ASSERT(resp != NULL);
    CU_ASSERT(resp->status_code == 200);
    buffer_free(resp);

    // header is blank
    resp = list_multipart_uploads(host, bucket, ak, sk, NULL, "", &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }
    buffer_free(resp);

    snprintf(header_str, 1024, "%s", "CONTENT-TYPE: text/plain");
    resp = list_multipart_uploads(host, bucket, ak, sk, NULL, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }
    buffer_free(resp);

    snprintf(header_str, 1024, "%s", "Content-Type: text/html;text/xml;text/plain");
    resp = list_multipart_uploads(host, bucket, ak, sk, NULL, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }
    buffer_free(resp);

    // abort uploadId finally
    char uploadIdStr[100] = { '\0' };
    snprintf(uploadIdStr, 100, "uploadId=%s", uploadId);
    resp = abort_multipart_upload(host, bucket, object_key, ak, sk, uploadId, NULL, NULL, &error);
    CU_ASSERT(resp != NULL);
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

void TEST_LIST_MULTIPART_UPLOADS_MULTI_TIMES(void) {
    // init multipart upload first
    char* argv[5];
    int count = 5, i = 0;
    for (; i < count; i++) {
        argv[i] = (char *) malloc (128);
        memset(argv[i], 0, 128);
        snprintf(object_key, 1024, "%s_%d", __FUNCTION__, i);
        buffer* resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
        CU_ASSERT(0 == error);
        CU_ASSERT(200 == resp->status_code);
        if (resp->status_code != 200) {
            printf("test %s:%d:\n", __FUNCTION__, __LINE__);
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);
        }
        GetUploadId(resp->body, argv[i]);
        CU_ASSERT(strlen(argv[i]) > 0);
        buffer_free(resp);
    }
    // list
    buffer* resp = list_multipart_uploads(host, bucket,
            ak, sk, NULL, NULL, &error); CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }
    buffer_free(resp);
    // list with max-uploads
    char query_args[100] = { '\0' };
    snprintf(query_args, 100, "uploads&max-uploads=3");
    resp = list_multipart_uploads(host, bucket,
            ak, sk, query_args, NULL, &error); CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }
    // check results, ret 3 uploadId, IsTruncated = true
    char truncated[5] = {'\0'};
    GetTruncated(resp->body, truncated);
    CU_ASSERT(strncmp(truncated, "true", strlen(truncated)) == 0);
    char upload_id_marker[100] = {'\0'};
    GetNextUploadIdMarker(resp->body, upload_id_marker);
    buffer_free(resp);
    // list with key-marker and upload-id-marker
    snprintf(query_args, 100, "UPLOADS&max-uploads=4"
            "&key-marker=TEST_LIST_MULTIPART_UPLOADS_MULTI_TIMES_2"
            "&upload-id-marker=%s", upload_id_marker);
    resp = list_multipart_uploads(host, bucket,
            ak, sk, query_args, NULL, &error); CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }
    // check results, ret 2 uploadId, and isTruncated = false
    char truncated2[5] = {'\0'};
    GetTruncated(resp->body, truncated2);
    CU_ASSERT(strncmp(truncated2, "false", strlen(truncated2)) == 0);
    buffer_free(resp);

    // abort uploadId finally
    for (i = 0; i < count; i++) {
        char uploadIdStr[100] = { '\0' };
        snprintf(object_key, 1024, "%s_%d", __FUNCTION__, i);
        snprintf(uploadIdStr, 100, "uploadId=%s", argv[i]);
        resp = abort_multipart_upload(host, bucket, object_key, ak, sk, argv[i], NULL, NULL, &error);
        CU_ASSERT(resp != NULL);
        CU_ASSERT(0 == error);
        CU_ASSERT(204 == resp->status_code);
        if (resp->status_code != 204) {
            printf("test %s:%d:\n", __FUNCTION__, __LINE__);
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);
        }
        buffer_free(resp);
        free(argv[i]);
    }
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
    if (NULL == CU_add_test(pSuite, "test list multipart uploads params all null\n",
                TEST_LIST_MULTIPART_UPLOADS_ALL_NULL)
        || NULL == CU_add_test(pSuite, "test list multipart uploads params return code null\n",
            TEST_LIST_MULTIPART_UPLOADS_RETURNCODE_NULL)
        || NULL == CU_add_test(pSuite, "test list multipart uploads params host\n",
            TEST_LIST_MULTIPART_UPLOADS_HOST)
        || NULL == CU_add_test(pSuite, "test list multipart uplaods params bucket\n",
            TEST_LIST_MULTIPART_UPLOADS_BUCKET)
        || NULL == CU_add_test(pSuite, "test list multipart uplaods params key\n",
            TEST_LIST_MULTIPART_UPLOADS_KEY)
        || NULL == CU_add_test(pSuite, "test list multipart uploads params query\n",
            TEST_LIST_MULTIPART_UPLOADS_QUERYPARA)
        || NULL == CU_add_test(pSuite, "test list multipart uploads params header\n",
            TEST_LIST_MULTIPART_UPLOADS_HEADERPARA)
        || NULL == CU_add_test(pSuite, "test list multipart uplaod multi times\n",
            TEST_LIST_MULTIPART_UPLOADS_MULTI_TIMES)
        || NULL == CU_add_test(pSuite, "test clean list multipart uploads\n",
            clean_bucket)) {
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


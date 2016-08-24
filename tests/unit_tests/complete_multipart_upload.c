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
char uploadid_str[128];
char access_key[128];
char secret_key[128];
char com_xml[4096];

const char *bucket = "bucket-test-for-complete-multipart-upload";

static int init_complete() {
    int result = 0;
    buffer* sub_resp = NULL;

    sub_resp = init_multipart_upload(host, bucket, object_key, ak, sk,
        NULL, NULL, &result);
    CU_ASSERT(0 == result);
    CU_ASSERT(200 == sub_resp->status_code);
    if (200 != sub_resp->status_code) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", sub_resp->status_code);
        printf("status msg = %s\n", sub_resp->status_msg);
        printf("error msg = %s\n", sub_resp->body);
        buffer_free(sub_resp);
        return 1;
    } else {
        char *oid_beg_ptr = strstr(sub_resp->body, "<UploadId>");
        if (oid_beg_ptr) {
            oid_beg_ptr += strlen("<UploadId>");
            char *oid_end_ptr = strstr(oid_beg_ptr, "</UploadId>");
            if (oid_end_ptr) {
                strncpy(uploadid_str, oid_beg_ptr, oid_end_ptr - oid_beg_ptr);
                uploadid_str[oid_end_ptr - oid_beg_ptr] = 0;
            }
        }
    }
    buffer_free(sub_resp);

    sub_resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "abcdefghijklmn", 14, NULL, NULL, &result);
    CU_ASSERT(0 == result);
    CU_ASSERT(200 == sub_resp->status_code);
    if (sub_resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", sub_resp->status_code);
        printf("status msg = %s\n", sub_resp->status_msg);
        printf("error msg = %s\n", sub_resp->body);
        buffer_free(sub_resp);
        return 2;
    }   
    
    char *etag_ptr = strstr(sub_resp->header, "ETag: \"");
    if (etag_ptr) {
        etag_ptr += strlen("ETag: \"");
    }else {
        buffer_free(sub_resp);
        return -2;
    }

    
    snprintf(com_xml, sizeof(com_xml), "<CompleteMultipartUpload>\n"
                                "<Part>\n"
                                "<PartNumber>%d</PartNumber>\n"
                                "<ETag>\"%.*s\"</ETag>"
                                "</Part>\n</CompleteMultipartUpload>",
                                1, 32, etag_ptr);

    buffer_free(sub_resp);

    return 0;
}

void TEST_COMPLETE_MULTIPART_UPLOAD_ALL_NULL(void) {
    resp = complete_multipart_upload(NULL, NULL, NULL, NULL, NULL, 
        NULL, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    
    return;
}

void TEST_COMPLETE_MULTIPART_UPLOAD_RETURNCODE_NULL(void) {
    
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        NULL, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = complete_multipart_upload(host, bucket, object_key, NULL, NULL,
        NULL, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    
    resp = complete_multipart_upload(host, bucket, NULL, NULL, NULL,
        NULL, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = complete_multipart_upload(NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    
    resp = complete_multipart_upload("", "", "", "", "",
        "", "", 0, "", "", NULL);
    CU_ASSERT(NULL == resp);
    
    return;
}

void TEST_COMPLETE_MULTIPART_UPLOAD_HOST(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    if (0 != init_complete()){
        CU_ASSERT(0);
        return ;
    }
    
    resp = complete_multipart_upload("", bucket, object_key, ak, sk, 
        uploadid_str, com_xml, strlen(com_xml), NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);
    
    resp = complete_multipart_upload(NULL, bucket, object_key, ak, sk, 
        uploadid_str, com_xml, strlen(com_xml), NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);

    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), NULL, NULL, &error);
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
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    return;
}
void TEST_COMPLETE_MULTIPART_UPLOAD_BUCKET(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    if (0 != init_complete() ){
        CU_ASSERT(0);
        return ;
    }
    // <Code>NotFoundApi</Code>
    resp = complete_multipart_upload(host, NULL, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    // <Code>NotFoundApi</Code>
    resp = complete_multipart_upload(host, "", object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    // <Code>NoSuchBucket</Code>
    resp = complete_multipart_upload(host, "test-test-test", object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    // <Code>NoSuchBucket</Code>
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), NULL, NULL, &error);
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
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    snprintf(object_key, 1024, "%s", __FUNCTION__);
     
    return;
}

void TEST_COMPLETE_MULTIPART_UPLOAD_OBJECT(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    if (0 != init_complete() ){
        CU_ASSERT(0);
        return ;
    }
    snprintf(query_str, sizeof(query_str), "" );
    // <Code>NotFoundApi</Code>
    resp = complete_multipart_upload(host, bucket, NULL, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    // <Code>NotFoundApi</Code>
    resp = complete_multipart_upload(host, bucket, "", ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    // <Code>NoSuchUpload</Code>
    resp = complete_multipart_upload(host, bucket, "1a2b3c", ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
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
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    return;
}
void TEST_COMPLETE_MULTIPART_UPLOAD_KEY(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    if (0 != init_complete() ){
        CU_ASSERT(0);
        return ;
    }
    query_str[0] = 0;
    
    resp = complete_multipart_upload(host, bucket, object_key, NULL, NULL,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = complete_multipart_upload(host, bucket, object_key, "", NULL,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = complete_multipart_upload(host, bucket, object_key, NULL, "",
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    
    resp = complete_multipart_upload(host, bucket, object_key, "", "",
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    
    resp = complete_multipart_upload(host, bucket, object_key, "sdfasdfasd",
        "7s9d7fasdf9asd89fasdfhsd", uploadid_str, 
        com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
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
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    return;
}
void TEST_COMPLETE_MULTIPART_UPLOAD_BUFDATAPARA(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    if (0 != init_complete() ){
        CU_ASSERT(0);
        return ;
    }
    snprintf(query_str, sizeof(query_str), "uploadId=%s", uploadid_str);

    char etag[32];
    char *etag_ptr = strstr(com_xml, "<ETag>\"");
    CU_ASSERT(NULL != etag_ptr);
    memcpy(etag, etag_ptr + strlen("<ETag>\""), 32);

    snprintf(com_xml, sizeof(com_xml), "<CompleteMultipartUpload>\n<Part>\n<PartNumber>%d</PartNumber>\n<ETag>\"%.*s\"</ETag>"
                                "</Part>\n</CompleteMultipartUpload>", 2, 32, etag);
    // <Code>InvalidPart</Code>    
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(400 == resp->status_code);
    if (resp->status_code != 400) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
#if 0    
    snprintf(com_xml, sizeof(com_xml), "<CompleteMultipartUpload>\n<Part>\n<PartNumber>%d</PartNumber>\n<ETag>\"abcdefgabcdefgabcdefgabcdefg1234\"</ETag>"
                                "</Part>\n</CompleteMultipartUpload>", 1);
        
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    snprintf(com_xml, sizeof(com_xml), "<CompleteMultipartUpload>\n<Part>\n<PartNumber>%d</PartNumber>\n<ETag>%.*s</ETag>"
                                "</Part>\n</CompleteMultipartUpload>", 1, 32, etag);
        
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
#endif
    snprintf(com_xml, sizeof(com_xml), "<CompleteMultipartUpload>\n<Part>\n<PartNumber>%d</PartNumber>\n<ETag>\"%.*s\"</ETag>"
                                "</Part>\n</CompleteMultipartUpload>", 1, 32, etag);
        
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
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
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    return;
}

void TEST_COMPLETE_MULTIPART_UPLOAD_QUERYPARA(void) {
    char md5_buf[32];
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    if (0 != init_complete() ){
        CU_ASSERT(0);
        return ;
    }
    snprintf(query_str, sizeof(query_str), "uploadId=%s", uploadid_str);
    
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    if (0 != init_complete() ){
        CU_ASSERT(0);
        return ;
    }
    snprintf(query_str, sizeof(query_str), "uploadid=%s", uploadid_str);
    //  
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    if (0 != init_complete() ){
        CU_ASSERT(0);
        return ;
    }
    snprintf(query_str, sizeof(query_str), "UPLOADID=%s", uploadid_str);
    //  
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 


    if (0 != init_complete() ){
        CU_ASSERT(0);
        return ;
    }
    // <Code>NoSuchUpload</Code>
    snprintf(query_str, sizeof(query_str), "uploadId=%s001", uploadid_str);
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, "", &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
    
    return;
}
void TEST_COMPLETE_MULTIPART_UPLOAD_HEADERPARA(void) {
    char md5_buf[32];
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    snprintf(header_str, 1024, "%s", "");
    if (0 != init_complete() ){
        CU_ASSERT(0);
        return ;
    }
    snprintf(query_str, sizeof(query_str), "uploadId=%s", uploadid_str);
#if 0 
    compute_buf_md5b64((char*)"0123456789", 10, md5_buf);
    snprintf(header_str, 1024, "Content-MD5: %s", md5_buf);
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk, com_xml, strlen(com_xml), query_str, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 
#endif
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk, 
        uploadid_str, com_xml, strlen(com_xml), query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    if (0 != init_complete(host, bucket, object_key, uploadid_str, com_xml, sizeof(com_xml)) ){
        CU_ASSERT(0);
        return ;
    }
    snprintf(query_str, sizeof(query_str), "uploadId=%s", uploadid_str);
    snprintf(header_str, 1024, "Content-Type: %s", "text/plain");
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk,
        uploadid_str, com_xml, strlen(com_xml), query_str, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    if (0 != init_complete(host, bucket, object_key, uploadid_str, com_xml, sizeof(com_xml)) ){
        CU_ASSERT(0);
        return ;
    }

    snprintf(query_str, sizeof(query_str), "uploadId=%s", uploadid_str);
    compute_buf_md5b64(com_xml, strlen(com_xml), md5_buf);
    snprintf(header_str, 1024, "CONTENT-Type: %s", "application/x-www-form-urlencoded");
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk, 
        uploadid_str, com_xml, strlen(com_xml), query_str, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(503 == resp->status_code);
    if (resp->status_code != 503) {                        
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);          
        printf("status code = %ld\n", resp->status_code);  
        printf("status msg = %s\n", resp->status_msg);     
        printf("error msg = %s\n", resp->body);            
    }                                                      
    buffer_free(resp);                                                                 

    resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(204 == resp->status_code);
    if (resp->status_code != 204) {                        
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
    if (NULL == CU_add_test(pSuite, "test complete multipart para null\n", TEST_COMPLETE_MULTIPART_UPLOAD_ALL_NULL)
        || NULL == CU_add_test(pSuite, "test complete multipart para return code null\n", TEST_COMPLETE_MULTIPART_UPLOAD_RETURNCODE_NULL)
        || NULL == CU_add_test(pSuite, "test complete multipart para host\n", TEST_COMPLETE_MULTIPART_UPLOAD_HOST)
        || NULL == CU_add_test(pSuite, "test complete multipart para bucket\n", TEST_COMPLETE_MULTIPART_UPLOAD_BUCKET)
        || NULL == CU_add_test(pSuite, "test complete multipart para object\n", TEST_COMPLETE_MULTIPART_UPLOAD_OBJECT)
        || NULL == CU_add_test(pSuite, "test complete multipart para key\n", TEST_COMPLETE_MULTIPART_UPLOAD_KEY)
        || NULL == CU_add_test(pSuite, "test complete multipart para data buf\n", TEST_COMPLETE_MULTIPART_UPLOAD_BUFDATAPARA)
        || NULL == CU_add_test(pSuite, "test complete multipart para query\n", TEST_COMPLETE_MULTIPART_UPLOAD_QUERYPARA)
        || NULL == CU_add_test(pSuite, "test complete multipart para header\n", TEST_COMPLETE_MULTIPART_UPLOAD_HEADERPARA)) {
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


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
char uploadid_str[128];

const char *bucket = "bucket-test-for-upload-part";

int init() {
    resp = init_multipart_upload(host, bucket, object_key,
        ak, sk, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (200 != resp->status_code) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);

        buffer_free(resp);
        return -1;
    }
    char *oid_beg_ptr = strstr(resp->body, "<UploadId>");
    if (oid_beg_ptr) {
        oid_beg_ptr += strlen("<UploadId>");
        char *oid_end_ptr = strstr(oid_beg_ptr, "</UploadId>");
        if (oid_end_ptr) {
            strncpy(uploadid_str, oid_beg_ptr, oid_end_ptr - oid_beg_ptr);
            uploadid_str[oid_end_ptr - oid_beg_ptr] = 0;
        }
    }
    buffer_free(resp);

    return 0;
}

void TEST_UPLOAD_PART_ALL_NULL(void) {
    resp = upload_part(NULL, NULL, NULL, NULL, NULL,
        NULL, 0, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    return;
}

void TEST_UPLOAD_PART_RETURNCODE_NULL(void) {

    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = upload_part(host, bucket, object_key, ak, sk,
        NULL, 0, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = upload_part(host, bucket, object_key, NULL, NULL,
        NULL, 0, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = upload_part(host, bucket, NULL, NULL, NULL,
        NULL, 0, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = upload_part(NULL, NULL, NULL, NULL, NULL,
        NULL, 0, NULL, 0, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);
    return;
}

void TEST_UPLOAD_PART_HOST(void) {

    snprintf(object_key, 1024, "%s", __FUNCTION__);
    error = init();
    CU_ASSERT(0 == error);

    resp = upload_part(NULL, bucket, object_key, ak, sk,
        uploadid_str, 1, "1234567890", 10, NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);

    resp = upload_part("", bucket, object_key, ak, sk,
        uploadid_str, 1, "1234567890", 10, NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);

    resp = upload_part("ks3-cn-shanghai.ksyun.com", bucket, object_key, ak, sk,
        uploadid_str, 1, "1234567890", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(307 == resp->status_code);
    if (resp->status_code != 307) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = upload_part("www.kingsoft.com", bucket, object_key, ak, sk,
        uploadid_str, 1, "1234567890", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code);
    if (resp->status_code != 405) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    // upload null object is allowed
    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "1234567890", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

/*
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
    */
    return;
}
void TEST_UPLOAD_PART_BUCKET(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    error = init();
    CU_ASSERT(0 == error);

    resp = upload_part(host, NULL, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(405 == resp->status_code); // HTTP/1.1 405 Request method 'PUT' not supported
    if (resp->status_code != 405) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    // <Code>NotFoundApi</Code>
    resp = upload_part(host, "", object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, NULL, NULL, &error);
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
    // <Code>NoSuchBucket</Code>
    // <Message>The specified bucket does not exist.</Message>
    resp = upload_part(host, "test-c-sdk-bucket", object_key, ak, sk,
        uploadid_str, 1, "123", 3, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, NULL, NULL, &error);
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

void TEST_UPLOAD_PART_OBJECT(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    error = init();
    CU_ASSERT(0 == error);

    resp = upload_part(host, bucket, NULL, ak, sk,
        uploadid_str, 1, "0123456789", 10,  NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = upload_part(host, bucket, "", ak, sk,
        uploadid_str, 1, "0123456789", 10,  NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = upload_part(host, bucket, "", ak, sk,
        uploadid_str, 1, "0123456789", 10,  NULL, NULL, &error);
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
void TEST_UPLOAD_PART_KEY(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    error = init();
    CU_ASSERT(0 == error);

    resp = upload_part(host, bucket, object_key, NULL, NULL,
        uploadid_str, 1, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = upload_part(host, bucket, object_key, ak, NULL,
        uploadid_str, 1, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = upload_part(host, bucket, object_key, NULL, sk,
        uploadid_str, 1, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = upload_part(host, bucket, object_key, "asdfghjkl", "qwertyuiop",
        uploadid_str, 1, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, NULL, NULL, &error);
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

void TEST_UPLOAD_PART_UPLOAD_PARTNUM(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    error = init();
    CU_ASSERT(0 == error);

    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 0, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 10001, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = upload_part(host, bucket, object_key, ak, sk,
        "", 1, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = upload_part(host, bucket, object_key, ak, sk,
        NULL, 1, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);
    // NoSuchUpload
    resp = upload_part(host, bucket, object_key, ak, sk,
        "asdfghjklmnbvcxz", 1, "0123456789", 10, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, NULL, NULL, &error);
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
void TEST_UPLOAD_PART_QUERYPARA(void) {
    objectid_str[0] = 0;
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    snprintf(query_str, 1024, "%s", "");

    error = init();
    CU_ASSERT(0 == error);

    // query_str no content, equal to upload_object
    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, "", NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    snprintf(query_str, 1024, "partNumber=%d&uploadId=%s1", 2, uploadid_str);
    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    snprintf(query_str, 1024, "partnumber=%d&uploadid=%s", 1, uploadid_str);
    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    snprintf(query_str, 1024, "partnumber=%d", 1);
    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    snprintf(query_str, 1024, "uploadid=%s", uploadid_str);
    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, query_str, NULL, &error);
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

void TEST_UPLOAD_PART_HEADERPARA(void) {
    char md5_buf[32];
    objectid_str[0] = 0;
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    snprintf(header_str, 1024, "%s", "");

    error = init();
    CU_ASSERT(0 == error);

    // query_str no content, equal to upload_object
    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, NULL, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    snprintf(header_str, 1024, "Content-MD5: %s", "itisincorrect==");
    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, query_str, header_str, &error);
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
    resp = upload_part(host, bucket, object_key, ak, sk,
        uploadid_str, 1, "0123456789", 10, query_str, header_str, &error);
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
    char sub_uploadid_str[1024];

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
            snprintf(sub_uploadid_str, 1024, "%.*s", key_len, key_ptr);
            snprintf(sub_query_str, 1024, "uploadId=%.*s", key_len, key_ptr);

            sub_resp = abort_multipart_upload(host, bucket, sub_object_key,
                ak, sk, sub_uploadid_str, sub_query_str, NULL, &error);
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
        || NULL == CU_add_test(pSuite, "test upload part para uploadid partnum\n", TEST_UPLOAD_PART_UPLOAD_PARTNUM)
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


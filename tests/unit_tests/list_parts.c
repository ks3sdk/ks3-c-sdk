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

const char *bucket = "bucket-test-for-list-parts";

static int init_list_parts(const char *host, const char *bucket,
        const char *object, const char* filename, char *upload_id) {
    int result = 0;
    buffer* sub_resp = NULL;
    part_result_node *result_arr = NULL;

    if (!upload_id ) return -1;

    upload_id[0] = 0;

    sub_resp = init_multipart_upload(host, bucket, object, ak, sk, NULL, NULL, &result);
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
                strncpy(upload_id, oid_beg_ptr, oid_end_ptr - oid_beg_ptr);
                upload_id[oid_end_ptr - oid_beg_ptr] = 0;
            }
        }
    }
    buffer_free(sub_resp);

    int part_num = 0;
    multiparts_upload(host, bucket, object, ak, sk, filename,
            upload_id, &result_arr, &part_num, &result, 3);
    if (result_arr)
        free(result_arr);
    result_arr = NULL;

    return part_num;
}

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

void TEST_LIST_PARTS_ALL_NULL(void) {
    resp = list_parts(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    return;
}

void TEST_LIST_PARTS_RETURNCODE_NULL(void) {

    snprintf(object_key, 1024, "%s", __FUNCTION__);
    resp = list_parts(host, bucket, object_key, ak, sk, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = list_parts(host, bucket, object_key, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = list_parts(host, bucket, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = list_parts(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CU_ASSERT(NULL == resp);

    resp = list_parts("", "", "", "", "", "", "", NULL, NULL);
    CU_ASSERT(NULL == resp);

    return;
}

void TEST_LIST_PARTS_HOST(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    const char* filename = "./list_parts";
    init_list_parts(host, bucket, object_key, filename, uploadid_str);

    snprintf(query_str, sizeof(query_str), "uploadId=%s", uploadid_str);
    resp = list_parts("", bucket, object_key, ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);

    resp = list_parts(NULL, bucket, object_key, ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(3 == error);
    buffer_free(resp);

    resp = list_parts("ks3-cn-shanghai.ksyun.com", bucket,
            object_key, ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(resp != NULL);
    if (resp != NULL) {
        CU_ASSERT(307 == resp->status_code);
        if (resp->status_code != 307) {
            printf("test %s:%d:\n", __FUNCTION__, __LINE__);
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);
        }
        buffer_free(resp);
    }

    resp = list_parts("www.kingsoft.com", bucket,
            object_key, ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(resp != NULL);
    if (resp != NULL) {
        CU_ASSERT(404 == resp->status_code);
        if (resp->status_code != 404) {
            printf("test %s:%d:\n", __FUNCTION__, __LINE__);
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);
        }
        buffer_free(resp);
    }

    resp = list_parts(host, bucket, object_key, ak, sk,
            uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_LIST_PARTS_BUCKET(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    const char* filename = "./list_parts";
    init_list_parts(host, bucket, object_key, filename, uploadid_str);

    snprintf(query_str, sizeof(query_str), "uploadId=%s", uploadid_str);
    resp = list_parts(host, "", object_key, ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = list_parts(host, NULL, object_key, ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = list_parts(host, "my-bucket", object_key, ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(307 == resp->status_code);
    if (resp->status_code != 307) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, NULL, NULL, &error);
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

void TEST_LIST_PARTS_OBJECT(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    const char* filename = "./list_parts";
    init_list_parts(host, bucket, object_key, filename, uploadid_str);
    // object is null, equal to list_bucket
    snprintf(query_str, sizeof(query_str), "uploadId=%s", uploadid_str);
    resp = list_parts(host, bucket, "", ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = list_parts(host, bucket, "", ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = list_parts(host, bucket, NULL, ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = list_parts(host, bucket, "123456789", ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(404 == resp->status_code);
    if (resp->status_code != 404) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
}

void TEST_LIST_PARTS_KEY(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    const char* filename = "./list_parts";
    init_list_parts(host, bucket, object_key, filename, uploadid_str);

    snprintf(query_str, sizeof(query_str), "uploadId=%s", uploadid_str);

    resp = list_parts(host, bucket, object_key, NULL, NULL, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = list_parts(host, bucket, object_key, "", NULL, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = list_parts(host, bucket, object_key, NULL, "", uploadid_str, NULL, NULL, &error);
    CU_ASSERT(-1 == error);
    CU_ASSERT(NULL == resp);

    resp = list_parts(host, bucket, object_key, "", "", uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = list_parts(host, bucket, object_key,
            "sdfasdfasd", "7s9d7fasdf9asd89fasdfhsd", uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(403 == resp->status_code);
    if (resp->status_code != 403) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, NULL, NULL, &error);
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

void TEST_LIST_PARTS_QUERYPARA(void) {
    const char* filename = ".bigfile.tmp";
    int next_num = 0;
    int max_part = 10;
    int part_num = 0;
    snprintf(object_key, 1024, "%s", __FUNCTION__);

    int64_t file_size = 100L << 20;
    CreateBigFile(filename, file_size);
    part_num = init_list_parts(host, bucket, object_key, filename, uploadid_str);
    RemoveFile(filename);

    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, "", NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    snprintf(query_str, sizeof(query_str), "uploadid=%s", uploadid_str);
    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    snprintf(query_str, sizeof(query_str), "UPLOADID=%s", uploadid_str);
    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    snprintf(query_str, sizeof(query_str), "uploadId=%s", uploadid_str);
    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    } else {
        char *content = resp->body;
        char *next_num_ptr = strstr(content, "<ns2:NextPartNumberMarker>");
        CU_ASSERT(NULL == next_num_ptr);

        char *par_ptr = strstr(content, "<ns2:Part>");
        CU_ASSERT(NULL != par_ptr);
        if (par_ptr) {
            char *beg = strstr(par_ptr, "<PartNumber>");
            beg += strlen("<PartNumber>");
            int num = atoi(beg);
            CU_ASSERT(1 == num);
        }
    }
    buffer_free(resp);

    snprintf(query_str, sizeof(query_str),
            "uploadId=%s&max-parts=%d&part-number-marker=%d", uploadid_str, max_part, 1);
    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    } else {
        char *content = resp->body;
        char *next_num_ptr = strstr(content, "<ns2:NextPartNumberMarker>");
        if (next_num_ptr) {
            next_num_ptr += strlen("<ns2:NextPartNumberMarker>");
            next_num = atoi(next_num_ptr);
        }
        CU_ASSERT( 11 == next_num);

        char *par_ptr = strstr(content, "<ns2:Part>");
        if (par_ptr) {
            char *beg = strstr(par_ptr, "<PartNumber>");
            beg += strlen("<PartNumber>");
            int num = atoi(beg);
            CU_ASSERT(1 == num);
            //CU_ASSERT(2 == num);
        }
    }
    buffer_free(resp);

    snprintf(query_str, sizeof(query_str),
            "uploadId=%s&max-parts=%d&part-number-marker=%d", uploadid_str, max_part, part_num +1);
    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, query_str, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    } else {
        char *content = resp->body;
        char *next_num_ptr = strstr(content, "<ns2:NextPartNumberMarker>");
        CU_ASSERT(NULL == next_num_ptr);

        char *par_ptr = strstr(content, "<ns2:Part>");
        CU_ASSERT(NULL == par_ptr);
    }
    buffer_free(resp);
}

void TEST_LIST_PARTS_HEADERPARA(void) {
    snprintf(object_key, 1024, "%s", __FUNCTION__);
    const char *filename = "list_parts";
    init_list_parts(host, bucket, object_key, filename, uploadid_str);

    snprintf(query_str, sizeof(query_str), "uploadId=%s", uploadid_str);

    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, NULL, NULL, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
        printf("test %s:%d:\n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);

    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, NULL, "", &error);
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
    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, NULL, header_str, &error);
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
    resp = list_parts(host, bucket, object_key, ak, sk, uploadid_str, NULL, header_str, &error);
    CU_ASSERT(0 == error);
    CU_ASSERT(200 == resp->status_code);
    if (resp->status_code != 200) {
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
    if (NULL == CU_add_test(pSuite, "test list parts para null\n",
                TEST_LIST_PARTS_ALL_NULL)
        || NULL == CU_add_test(pSuite, "test list parts para return code null\n",
            TEST_LIST_PARTS_RETURNCODE_NULL)
        || NULL == CU_add_test(pSuite, "test list parts para host\n",
            TEST_LIST_PARTS_HOST)
        || NULL == CU_add_test(pSuite, "test list parts para bucket\n",
            TEST_LIST_PARTS_BUCKET)
        || NULL == CU_add_test(pSuite, "test list parts para object\n",
            TEST_LIST_PARTS_OBJECT)
        || NULL == CU_add_test(pSuite, "test list parts para key\n",
            TEST_LIST_PARTS_KEY)
        || NULL == CU_add_test(pSuite, "test list parts para query\n",
            TEST_LIST_PARTS_QUERYPARA)
        || NULL == CU_add_test(pSuite, "test list parts para header\n",
            TEST_LIST_PARTS_HEADERPARA)
        || NULL == CU_add_test(pSuite, "test clean list parts\n",
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


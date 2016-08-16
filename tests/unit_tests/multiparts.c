
#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "api.h"
#include "md5.h"
#include "./load_key.h"
#include "multiparts.h"

const char* bucket = "bucket-test-for-multiparts05";

const long   FIRST_MIN_PART_SIZE = (5 * 1024 * 1024); 
const long   SECOND_MIN_PART_SIZE = (100 * 1024); 


int encode_md5hex_b64(const char* md5_hex, char *b64) {
    unsigned char tmp[256];
    int tmp_len = 0;
    unsigned char n1, n2;
    while (*md5_hex && *(md5_hex + 1)) {
        if (*md5_hex >= 'a' && *md5_hex <= 'f')
            n1 = *md5_hex - 'a' + 10;
        else
            n1 = *md5_hex - '0';

        md5_hex++;

        if (*md5_hex >= 'a' && *md5_hex <= 'f')
            n1 = *md5_hex - 'a' + 10;
        else
            n1 = *md5_hex - '0';

        tmp[tmp_len++] = n1 << 4 + n2;
    }

    return b64_encode(tmp, tmp_len, b64);
}

void test_base64() {
    char tmp[] = "hello, everyone!";
    int len = strlen(tmp);

    char out_buf[128] = { 0 };

    char out_src[128] = { 0 };

    b64_encode((unsigned char*)tmp, len, out_buf);

    int length = b64_decode(out_buf, out_src);

    if (length == len && 0 == strncmp(tmp, out_src, len))
        printf("[ERROR] compute base64 failed, please review code.\n");

    return ;
}


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

void TEST_MULTIPARTS_COMPLETE_00(void) {
    int error;
    const char* filename = "./lib/libcunit.a";

    buffer *resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char objectid_str[128] = { 0 };

    strcpy(query_str, "uploads");
    strcpy(object_key, "test_multiparts_object_00");
    

    // 1. init_multipart
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    if (200 != resp->status_code) {
        printf("create_bucket:\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);

    char *oid_beg_ptr = strstr(resp->body, "<UploadId>");
    if (oid_beg_ptr) {
        oid_beg_ptr += strlen("<UploadId>");
        char *oid_end_ptr = strstr(oid_beg_ptr, "</UploadId>");
        if (oid_end_ptr) {
            strncpy(objectid_str, oid_beg_ptr, oid_end_ptr - oid_beg_ptr);
            objectid_str[oid_end_ptr - oid_beg_ptr] = 0;
        }
    }
    buffer_free(resp);
    printf("host:%s\nbucket:%s\nobject:%s\nUploadId:%s\n", host, bucket, object_key, objectid_str);

    // 2. upload part
    part_result_node *part_result_arr = NULL;
    int part_result_num = 0;
    int part_num = multiparts_upload(host, bucket, object_key, ak, sk, filename, objectid_str,
        &part_result_arr, &part_result_num, &error, 3);
    CU_ASSERT(part_num == 0);
    if (part_result_arr)
        free(part_result_arr);
    part_result_arr = NULL;
    
    // 3. list_multipart_uploads
    snprintf(query_str, 1024, "uploads");
    resp = list_multipart_uploads(host, bucket, ak, sk, NULL, NULL, &error);

    if (resp->status_code != 200) {
        printf("test list_multipart_uploads:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);

    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    // 4. list_parts
    snprintf(query_str, 1024, "uploadId=%s", objectid_str);
    resp = list_parts(host, bucket, object_key, ak, sk, query_str, NULL, &error);

    if (resp->status_code != 200) {
        printf("test list_parts:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);

    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);

    // 5. complete_multipart_upload
    int buf_size = 5 * 1024 * 1024;
    char* buf = (char *)malloc(buf_size);
    if (!buf) {
        printf("[ERROR] alloc memory failed! please review code.");
        CU_ASSERT(0);
        return;
    }

    int dat_len = 0;
    int dat_size = buf_size;
    int print_len = 0;
    char *dat = buf;

    print_len = snprintf(dat + dat_len, dat_size - dat_len, "<CompleteMultipartUpload>\n");
    dat_len += print_len;

    char *content = resp->body;
    char *par_ptr = strstr(content, "<ns2:Part>");

    while (par_ptr) {
        char *beg = strstr(par_ptr, "<PartNumber>");
        char *end = strstr(par_ptr, "</ETag>");
        end += strlen("</ETag>");
        print_len = snprintf(dat + dat_len, dat_size - dat_len, "<Part>\n%.*s\n</Part>\n", (int)(end - beg), beg);
        dat_len += print_len;
        content = end;
        par_ptr = strstr(content, "<ns2:Part>");
    }
    print_len = snprintf(dat + dat_len, dat_size - dat_len, "</CompleteMultipartUpload>\n");
    dat_len += print_len;
    buffer_free(resp);

    snprintf(query_str, 1024, "uploadId=%s", objectid_str);
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk, dat, dat_len, query_str, NULL, &error);

    if (resp->status_code != 200) {
        printf("test complete_multipart_upload:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        //return;
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    if (resp->status_code != 204) {
        printf("test delete_object:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        //return;
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);

    free(buf);

    return;
}

void TEST_MULTIPARTS_COMPLETE_01(void) {
    int error;
    //const char* filename = "/data/ssd4/liangjianqun/migrate/ks3/all/round1-1.tar.gz";
    const char* filename = "./lib/libcunit.a";

    buffer *resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char uploadid_str[128] = { 0 };

    strcpy(query_str, "uploads");
    strcpy(object_key, "test_multiparts_object_01");


    // 1. init_multipart
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    if (200 != resp->status_code) {
        printf("create_bucket:\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);

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

    printf("host:%s\nbucket:%s\nobject:%s\nUploadId:%s\n", host, bucket, object_key, uploadid_str);

    // 2. upload part
    part_result_node *part_result_arr = NULL;
    int part_result_num = 0;
    int part_num = multiparts_upload(host, bucket, object_key, ak, sk, filename, uploadid_str,
        &part_result_arr, &part_result_num, &error, 3);

    CU_ASSERT(part_num == 0);
    
    // complete_multipart_upload
    long buf_size = 5 * 1024 * 1024;
    char* buf = (char *)malloc(buf_size);
    if (!buf) {
        printf("[ERROR] alloc memory failed! please review code.");
        return;
    }
    int dat_len = 0;
    int dat_size = buf_size;
    int print_len = 0;
    char *dat = buf;

    print_len = snprintf(dat + dat_len, dat_size - dat_len, "<CompleteMultipartUpload>\n");
    dat_len += print_len;
    int i=0;
    for (i = 1; i < part_result_num; ++i) {
        print_len = snprintf(dat + dat_len, dat_size - dat_len, "<Part><PartNumber>%d</PartNumber><ETag>\"%.*s\"</ETag></Part>",
            part_result_arr[i - 1].id, ETAG_LEN, part_result_arr[i - 1].etag);
        dat_len += print_len;
    }   
    print_len = snprintf(dat + dat_len, dat_size - dat_len, "</CompleteMultipartUpload>\n");
    dat_len += print_len;

    snprintf(query_str, 1024, "uploadId=%s", uploadid_str);
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk, dat, dat_len, query_str, NULL, &error);

    if (resp->status_code != 200) {
        printf("test complete_multipart_upload:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);        
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    if (part_result_arr)
        free(part_result_arr);
    part_result_arr = NULL;

    // delete_object
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

    if (buf)
        free(buf);
    buf = NULL;
    if (part_result_arr)
        free(part_result_arr);
    part_result_arr = NULL;

    return;
}

void TEST_MULTIPARTS_ABORT_02(void) {
    int error;
    const char* filename = "./lib/libcunit.a";

    buffer *resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char objectid_str[128] = { 0 };

    strcpy(object_key, "test_multiparts_object_02");
    strcpy(query_str, "uploads");
    strcpy(header_str, "Content-Type: text/plain;charset=UTF-8");

    // 1. init_multipart
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, header_str, &error);
    if (200 != resp->status_code) {
        printf("create_bucket:\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);    

    char *oid_beg_ptr = strstr(resp->body, "<UploadId>");
    if (oid_beg_ptr) {
        oid_beg_ptr += strlen("<UploadId>");
        char *oid_end_ptr = strstr(oid_beg_ptr, "</UploadId>");
        if (oid_end_ptr) {
            strncpy(objectid_str, oid_beg_ptr, oid_end_ptr - oid_beg_ptr);
            objectid_str[oid_end_ptr - oid_beg_ptr] = 0;
        }
    }
    buffer_free(resp);

    // 2. upload part
    part_result_node *part_result_arr = NULL;
    int part_result_num = 0;
    int part_num = multiparts_upload(host, bucket, object_key, ak, sk, filename, objectid_str,
        &part_result_arr, &part_result_num, &error, 3);
    CU_ASSERT(part_num == 0);
    if (part_result_arr)
        free(part_result_arr);
    part_result_arr = NULL;

    // 3. list_multipart_uploads
    snprintf(query_str, 1024, "uploads");
    resp = list_multipart_uploads(host, bucket, ak, sk, NULL, NULL, &error);

    if (resp->status_code != 200) {
        printf("test list_multipart_uploads:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);

    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    // 4. list_parts
    snprintf(query_str, 1024, "uploadId=%s", objectid_str);
    resp = list_parts(host, bucket, object_key, ak, sk, query_str, NULL, &error);

    if (resp->status_code != 200) {
        printf("test list_parts:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);

    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);

    // 5. abort_multipart_upload
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

void TEST_LIST_PARTS_03(void) {
    int error;
    //const char* filename = "./lib/libcunit.a"; 
    const char* filename = "/data/ssd4/liangjianqun/migrate/ks3/all/round1-2.tar.gz";

    buffer *resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char upload_id[128] = { 0 };

    strcpy(object_key, "test_multiparts_object_03");
    strcpy(query_str, "uploads");
    strcpy(header_str, "Content-Type: text/plain;charset=UTF-8");

    // 1. init_multipart
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, header_str, &error);
    if (200 != resp->status_code) {
        printf("create_bucket:\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);

    char *oid_beg_ptr = strstr(resp->body, "<UploadId>");
    if (oid_beg_ptr) {
        oid_beg_ptr += strlen("<UploadId>");
        char *oid_end_ptr = strstr(oid_beg_ptr, "</UploadId>");
        if (oid_end_ptr) {
            strncpy(upload_id, oid_beg_ptr, oid_end_ptr - oid_beg_ptr);
            upload_id[oid_end_ptr - oid_beg_ptr] = 0;
        }
    }
    buffer_free(resp);

    printf("host:%s\nbucket:%s\nobject:%s\nUploadId:%s\n", host, bucket, object_key, upload_id);

    // 2. upload part
    part_result_node *part_result_arr = NULL;
    int part_result_num = 0;
    int part_num = multiparts_upload(host, bucket, object_key, ak, sk, filename, upload_id,
        &part_result_arr, &part_result_num, &error, 3);
    CU_ASSERT(part_num == 0);
    if (part_result_arr)
        free(part_result_arr);
    part_result_arr = NULL;

    // 3. list_multipart_uploads
    snprintf(query_str, 1024, "uploads");
    resp = list_multipart_uploads(host, bucket, ak, sk, NULL, NULL, &error);

    if (resp->status_code != 200) {
        printf("test list_multipart_uploads:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);

    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);
 
    // complete_multipart_upload
    int buf_size = 5 * 1024 * 1024;
    char* buf = (char *)malloc(buf_size);
    if (!buf) {
        printf("[ERROR] alloc memory failed! please review code.");
        CU_ASSERT(0);
        return;
    }
    int dat_len = 0;
    int dat_size = buf_size;
    int print_len = 0;
    char *dat = buf;

    int next_num = 1;
    int keep_on = 0;
    int part_num_per = 100;

    print_len = snprintf(dat + dat_len, dat_size - dat_len, "<CompleteMultipartUpload>\n");
    dat_len += print_len;
    
    // list_parts
    do {
        keep_on = 0;
        snprintf(query_str, 1024, "uploadId=%s&max-parts=%d&part-number-marker=%d", upload_id, part_num_per, next_num);
        resp = list_parts(host, bucket, object_key, ak, sk, query_str, NULL, &error);

        if (resp->status_code != 200) {
            printf("test list_parts:\n");
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);

        }
        CU_ASSERT(error == 0);
        CU_ASSERT(200 == resp->status_code);

        char *content = resp->body;
        char *next_num_ptr = strstr(content, "<ns2:NextPartNumberMarker>");
        if (next_num_ptr) {
            next_num_ptr += strlen("<ns2:NextPartNumberMarker>");
            next_num = atoi(next_num_ptr);
            if (next_num <= part_result_num) {
                keep_on = 1;
            }
        }
        
        char *par_ptr = strstr(content, "<ns2:Part>");
        while (par_ptr) {
            char *beg = strstr(par_ptr, "<PartNumber>");
            char *end = strstr(par_ptr, "</ETag>");
            end += strlen("</ETag>");
            print_len = snprintf(dat + dat_len, dat_size - dat_len, "<Part>\n%.*s\n</Part>\n", (int)(end - beg), beg);
            dat_len += print_len;
            content = end;
            par_ptr = strstr(content, "<ns2:Part>");
        }        
        buffer_free(resp);
    } while (keep_on);

    print_len = snprintf(dat + dat_len, dat_size - dat_len, "</CompleteMultipartUpload>\n");
    dat_len += print_len;
    snprintf(query_str, 1024, "uploadId=%s", upload_id);    
    snprintf(header_str, 1024, "Content-Type: application/xml");
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk, dat, dat_len, query_str, header_str, &error);
    if (resp->status_code != 200) {
        printf("test complete_multipart_upload:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);
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

    free(buf);
    return;
}

void TEST_MULTIPARTS_UPLOAD_DOWNLOAD_04(void) {
    int error;
    const char* filename = "./lib/libcunit.a"; 
    //const char* filename = "/data/ssd4/liangjianqun/migrate/ks3/all/round1-1.tar.gz";
    char down_filename[1024];

    buffer *resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char upload_id[128] = { 0 };

    strcpy(object_key, "test_multiparts_object_04");
    strcpy(query_str, "uploads");
    strcpy(header_str, "Content-Type: text/plain;charset=UTF-8");

    // 1. init_multipart
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, header_str, &error);
    if (200 != resp->status_code) {
        printf("create_bucket:\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);

    char *oid_beg_ptr = strstr(resp->body, "<UploadId>");
    if (oid_beg_ptr) {
        oid_beg_ptr += strlen("<UploadId>");
        char *oid_end_ptr = strstr(oid_beg_ptr, "</UploadId>");
        if (oid_end_ptr) {
            strncpy(upload_id, oid_beg_ptr, oid_end_ptr - oid_beg_ptr);
            upload_id[oid_end_ptr - oid_beg_ptr] = 0;
        }
    }
    buffer_free(resp);

    printf("host:%s\nbucket:%s\nobject:%s\nUploadId:%s\n", host, bucket, object_key, upload_id);

    // 2. upload part
    part_result_node *part_result_arr = NULL;
    int part_result_num = 0;
    int part_num = multiparts_upload(host, bucket, object_key, ak, sk, filename, upload_id,
        &part_result_arr, &part_result_num, &error, 3);
    CU_ASSERT(part_num == 0);
    if (part_result_arr)
        free(part_result_arr);
    part_result_arr = NULL;

    // complete_multipart_upload
    int buf_size = 5 * 1024 * 1024;
    char* buf = (char *)malloc(buf_size);
    if (!buf) {
        printf("[ERROR] alloc memory failed! please review code.");
        CU_ASSERT(0);
        return;
    }
    int dat_len = 0;
    int dat_size = buf_size;
    int print_len = 0;
    char *dat = buf;

    int next_num = 1;
    int keep_on = 0;

    print_len = snprintf(dat + dat_len, dat_size - dat_len, "<CompleteMultipartUpload>\n");
    dat_len += print_len;

    // list_parts
    do {
        keep_on = 0;
        snprintf(query_str, 1024, "uploadId=%s&max-parts=%d&part-number-marker=%d", upload_id, 300, next_num);
        resp = list_parts(host, bucket, object_key, ak, sk, query_str, NULL, &error);

        if (resp->status_code != 200) {
            printf("test list_parts: %s:%d\n", __FUNCTION__, __LINE__);
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);

        }
        CU_ASSERT(error == 0);
        CU_ASSERT(200 == resp->status_code);

        char *content = resp->body;
        char *next_num_ptr = strstr(content, "<ns2:NextPartNumberMarker>");
        if (next_num_ptr) {
            next_num_ptr += strlen("<ns2:NextPartNumberMarker>");
            next_num = atoi(next_num_ptr);
            if (next_num <= part_result_num) {
                keep_on = 1;
                next_num--;
            }
        }

        char *par_ptr = strstr(content, "<ns2:Part>");
        while (par_ptr) {
            char *beg = strstr(par_ptr, "<PartNumber>");
            char *end = strstr(par_ptr, "</ETag>");
            end += strlen("</ETag>");
            print_len = snprintf(dat + dat_len, dat_size - dat_len, "<Part>\n%.*s\n</Part>\n", (int)(end - beg), beg);
            dat_len += print_len;
            content = end;
            par_ptr = strstr(content, "<ns2:Part>");
        }
        buffer_free(resp);
    } while (keep_on);

    print_len = snprintf(dat + dat_len, dat_size - dat_len, "</CompleteMultipartUpload>\n");
    dat_len += print_len;

    snprintf(query_str, 1024, "uploadId=%s", upload_id);
    snprintf(header_str, 1024, "Content-Type: application/xml");
    resp = complete_multipart_upload(host, bucket, object_key, ak, sk, dat, dat_len, query_str, header_str, &error);

    if (resp->status_code != 200) {
        printf("test complete_multipart_upload: %s:%d \n", __FUNCTION__, __LINE__);
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        //return;
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    snprintf(down_filename, 1024, "%s.download", filename);
    resp = download_file_object(host, bucket, object_key, down_filename, ak, sk, NULL, NULL, &error);
    if (resp->status_code != 200) {
        printf("test download_file_object:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(200 == resp->status_code);
    buffer_free(resp);

    char b64_up[64];
    char b64_down[64];
    compute_file_md5b64(filename, b64_up);
    compute_file_md5b64(down_filename, b64_down);
    int cmp_ret = strcmp(b64_up, b64_down);
    if (cmp_ret == 0) {
        printf("[ok] %s:%d upload file == download file\n",  __FUNCTION__, __LINE__);
    }
    CU_ASSERT(cmp_ret == 0);

    resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    if (resp->status_code != 204) {
        printf("test delete_object:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
        //return;
    }
    CU_ASSERT(error == 0);
    CU_ASSERT(204 == resp->status_code);
    buffer_free(resp);

    free(buf);

    return;
}

void TEST_LIST_PARTS_05(void) {
    int error;
    buffer *resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char upload_id[128] = { 0 };

    strcpy(object_key, "test_multiparts_object_03");
    strcpy(header_str, "Content-Type: text/plain;charset=UTF-8");
    strcpy(upload_id, "18c233bab07a49a99c73c54b9ba54296");

    int next_num = 1;
    int keep_on = 0;
    int part_num = 1000;

    // list_parts
    do {
        keep_on = 0;
        snprintf(query_str, 1024, "uploadId=%s&max-parts=%d&part-number-marker=%d", upload_id, part_num, next_num);
        resp = list_parts(host, bucket, object_key, ak, sk, query_str, NULL, &error);
        if (resp->status_code != 200) {
            printf("test list_parts:\n");
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);

        }
        char *content = resp->body;
        char *next_num_ptr = strstr(content, "<ns2:NextPartNumberMarker>");
        if (next_num_ptr) {
            next_num_ptr += strlen("<ns2:NextPartNumberMarker>");
            next_num = atoi(next_num_ptr);
            if (next_num <= 767) {
                keep_on = 1;
                //next_num--;
            }
        }
        
        buffer_free(resp);
    } while (keep_on);

    return;
}

void TEST_LIST_MULTIPARTS(void) {
    int error;
    buffer *resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char upload_id[128] = { 0 };
    int obj_num = 1000;

    int i = 0;
    for (i = 0; i < obj_num; ++i ) { 
        snprintf(object_key, 1024, "test_multiparts_object_%04d", i);
        snprintf(query_str, 1024, "uploads");
        snprintf(header_str, 1024, "Content-Type: text/plain;charset=UTF-8");

        // 1. init_multipart
        resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, header_str, &error);
        if (200 != resp->status_code) {
            printf("create_bucket:\nstatus code=%ld\n", resp->status_code);
            printf("status msg=%s\n", resp->status_msg);
            printf("error msg=%s\n", resp->body);
        }
        CU_ASSERT(error == 0);
        CU_ASSERT(200 == resp->status_code);

        buffer_free(resp);
    }
        
    obj_num = obj_num << 10;

    int key_num = 0;
    int buf_size = obj_num * 256 ;
    char *buf = (char*)malloc(buf_size);
    int  *off_arr = (int *)malloc(obj_num * 2 * sizeof(int));
    char *cur = buf;

    // list_multipart_uploads
    int max_uploads = 200;
    char upload_id_marker[1024];
    char key_marker[1024];
    int has_next = 0;
    char *key_ptr = NULL;
    char *key_end = NULL;
    int key_len = 0;
    
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

            off_arr[key_num++] = cur - buf;
            strncpy(cur, key_ptr, key_len);
            cur += key_len;
            *(short *)cur = 0;
            cur += 2;
            
            key_ptr = strstr(key_end, "<UploadId>");
            key_ptr += strlen("<UploadId>");
            key_end = strstr(key_ptr, "</UploadId>");
            key_len = key_end - key_ptr;
                
            off_arr[key_num++] = cur - buf;
            strncpy(cur, key_ptr, key_len);
            cur += key_len;
            *(short *)cur = 0;
            cur += 2; 
            
            key_ptr = strstr(key_end, "<Key>");
        }
        
        buffer_free(resp);
    } while (has_next);

    printf("key_num is %d\n", key_num);

    i = 0; 
    while ( i < key_num ) {
        // 5. abort_multipart_upload
        snprintf(object_key, 1024, "%s", buf + off_arr[i++]);
        snprintf(query_str, 1024, "uploadId=%s", buf + off_arr[i++]);

        resp = abort_multipart_upload(host, bucket, object_key, ak, sk, query_str, NULL, &error);
        if (resp->status_code != 204) {
            printf("test abort_multipart_upload: i = %d\n", i - 2);
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);
        }
        CU_ASSERT(error == 0);
        CU_ASSERT(204 == resp->status_code);
        buffer_free(resp);
    }

    free(buf);
    free(off_arr);

    return;
}


void TEST_LIST_MULTIPARTS_01(void) {
    int error;
    buffer *resp = NULL;
    buffer *sub_resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char upload_id[128] = { 0 };
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
    char *upload_ptr = NULL;
    char *upload_end = NULL;
    int   upload_len = 0;
    const int obj_num = 1000;
    int i = 0;
    
    for (i = 0; i < obj_num; ++i ) { 
        snprintf(object_key, 1024, "test_multiparts_object_1%04d", i);
        snprintf(query_str, 1024, "uploads");
        snprintf(header_str, 1024, "Content-Type: text/plain;charset=UTF-8");

        // 1. init_multipart
        resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, header_str, &error);
        if (200 != resp->status_code) {
            printf("create_bucket:\nstatus code=%ld\n", resp->status_code);
            printf("status msg=%s\n", resp->status_msg);
            printf("error msg=%s\n", resp->body);
        }
        CU_ASSERT(error == 0);
        CU_ASSERT(200 == resp->status_code);

        buffer_free(resp);
    }

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
    test_base64();

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
    if (NULL == CU_add_test(pSuite, "test multi parts complete 00\n", TEST_MULTIPARTS_COMPLETE_00)
        || NULL == CU_add_test(pSuite, "test multi parts complete by list 01\n", TEST_MULTIPARTS_COMPLETE_01)
        || NULL == CU_add_test(pSuite, "test multi parts abort 02\n", TEST_MULTIPARTS_ABORT_02)
        || NULL == CU_add_test(pSuite, "test list parts 03\n", TEST_LIST_PARTS_03)
        || NULL == CU_add_test(pSuite, "test multi parts upload and download 04\n", TEST_MULTIPARTS_UPLOAD_DOWNLOAD_04)
        || NULL == CU_add_test(pSuite, "test list multi part\n", TEST_LIST_MULTIPARTS)
        || NULL == CU_add_test(pSuite, "test list multi part plus\n", TEST_LIST_MULTIPARTS_01) ) {
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

/*************************************************************************
	> File Name: load_key.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Wed 18 May 2016 04:16:06 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./load_key.h"
#include "api.h"

int load_ak_sk() {
    char* key_file = "/tmp/key_test_sdk";
    FILE* fp = NULL;
    int len = 0;
    fp = fopen(key_file, "r");
    if (fp == NULL) {
        printf("fopen file=%s failed\n", key_file);
        return -1;
    }
    
    fgets(ak, 100, fp);
    len = strlen(ak);
    while (len > 0 && (ak[len - 1] == '\r' || ak[len -1] == '\n'))
        ak[--len] = 0;
    printf("ak=%s\n", ak);
    
    fgets(sk, 100, fp);
    len = strlen(sk);
    while (len > 0 && (sk[len - 1] == '\r' || sk[len -1] == '\n'))
        sk[--len] = 0;
    printf("sk=%s\n", sk);

    fgets(host, 512, fp);
    len = strlen(host);
    while (len > 0 && (host[len - 1] == '\r' || host[len -1] == '\n'))
        host[--len] = 0;
    printf("host=%s\n", host);
    
    fclose(fp);
    return 0;
}

int CreateBucket(const char* host, const char* bucket) {
    int error;
    buffer* resp = NULL;

    resp = create_bucket(host, bucket, ak, sk, NULL, &error);
    if (error != 0) {
        return error;
    }
    if (resp->status_code != 200 && resp->status_code != 409) {
        printf("status code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
        buffer_free(resp);
        return resp->status_code;
    }
        printf("status code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    buffer_free(resp);
    return 0;
}

int DeleteBucket(const char* host, const char* bucket) {
    int error;
    buffer* resp = NULL;

    resp = delete_bucket(host, bucket, ak, sk, NULL, &error);
    if (error != 0) {
        return error;
    }
    if (resp->status_code != 204 && resp->status_code != 404) {
        printf("status code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
        buffer_free(resp);
        return resp->status_code;
    }
    buffer_free(resp);
    printf("bucket will actually delete after several minutes(>=5)\n");
    return 0;
}

void GetUploadId(const char* in_str, char* out_str) {
    char *oid_beg_ptr = strstr(in_str, "<UploadId>");
    if (oid_beg_ptr) {
        oid_beg_ptr += strlen("<UploadId>");
        char *oid_end_ptr = strstr(oid_beg_ptr, "</UploadId>");
        if (oid_end_ptr) {
            strncpy(out_str, oid_beg_ptr, oid_end_ptr - oid_beg_ptr);                                            
            out_str[oid_end_ptr - oid_beg_ptr] = 0;
        }
    }
}

const char* CreateBigFile(const char* file, int64_t file_size) {
    FILE* fd = NULL;
    unsigned char buf[4096];
    int part_num = 0;
    int i = 0;

    srand((unsigned)time(NULL));
    for (i = 0; i < part_num; ++i)
        buf[i] = rand() % 256;

    fd = fopen(file, "wb");
    while(file_size > 0 ) {
        if (file_size > sizeof(buf)) 
            part_num = sizeof(buf);
        else
            part_num = file_size;
        
        i = rand() % part_num;
        buf[i] = rand() % 256;
        
        fwrite(buf, part_num, 1, fd);
        file_size -= part_num;        
    }
    fclose(fd);
    return file;
}

void RemoveFile(const char *file) {
    int ret = 0;
    while(access(file, 0) == 0) {
        ret = remove(file);
        if (ret) {
            fprintf(stderr, "remove %s failed!\n", file);
            sleep(1000);
        }
    }
    return ;
}

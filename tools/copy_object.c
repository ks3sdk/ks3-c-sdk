/*************************************************************************
	> File Name: delete_bucket.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Fri 20 May 2016 10:36:54 AM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include "api.h"

char* bj_host = "ks3-cn-beijing.ksyun.com";
char* hz_host = "kss.ksyun.com";
char* sh_host = "ks3-cn-shanghai.ksyun.com";
char* hk_host = "ks3-cn-hk-1.ksyun.com";
char* skll_host = "ks3-us-west-1.ksyun.com";

char ak[100];
char sk[100];

int load_key();

int main(int argc, char* argv[]) {
    if (argc != 6) {
        printf("[Usage] %s [region(bj/hz/sh/hk/skll)] [src_bucket]"
                " [src_object] [dst_bucket] [dst_object]\n", argv[0]);
        return 0;
    }
    int ret = load_key();
    if (ret != 0) {
        printf("[ERROR] load key failed\n");
        return ret;
    }

    char* region = argv[1];
    char* src_bucket = argv[2];
    char* src_object = argv[3];
    char* dst_bucket = argv[4];
    char* dst_object = argv[5];
    char* host = NULL;
    if (strncmp(region, "bj", strlen("bj")) == 0) {
        host = bj_host;
    } else if (strncmp(region, "hz", strlen("hz")) == 0) {
        host = hz_host;
    } else if (strncmp(region, "sh", strlen("sh")) == 0) {
        host = sh_host;
    } else if (strncmp(region, "hk", strlen("hk")) == 0) {
        host = hk_host;
    } else if (strncmp(region, "skll", strlen("skll")) == 0) {
        host = skll_host;
    } else {
        printf("[ERROR] unknown region %s\n", region);
        return 0;
    }

    int error;
    buffer* resp = NULL;

    resp = copy_object(host, src_bucket, src_object, dst_bucket,
            dst_object, ak, sk, NULL, NULL, &error);
    if (error != 0) {
        printf("curl error=%d\n", error);
        return error;
    }

    if (resp->status_code == 200) {
        printf("[OK] copy object [%s:%s]->[%s:%s] ok\n",
                src_bucket, src_object, dst_bucket, dst_object);
    } else {
        printf("[ERROR] status_code=%d\n", resp->status_code);
        printf("[ERROR] status_msg=%s\n", resp->status_msg);
        printf("[ERROR] err_msg=%s\n", resp->body);
    }
    buffer_free(resp);
    return 0;
}

int load_key() {
    // load_key
    char* key_file = "/home/hanbing1/key";
    FILE* fp = NULL;
    fp = fopen(key_file, "r");
    if (fp == NULL) {
        printf("fopen file=%s failed\n", key_file);
        return -1;
    }
    fgets(ak, 100, fp);
    int len = strlen(ak);
    ak[len - 1] = '\0';
    printf("ak=%s\n", ak);
    fgets(sk, 100, fp);
    len = strlen(sk);
    sk[len - 1] = '\0';
    printf("sk=%s\n", sk);
    fclose(fp);
    return 0;
}

/*************************************************************************
	> File Name: delete_bucket.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Fri 20 May 2016 10:36:54 AM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include "ks3.h"

const char* bj_host = "ks3-cn-beijing-internal.ksyun.com";
const char* gz_host = "ks3-cn-guangzhou-internal.ksyun.com";
const char* sh_host = "ks3-cn-shanghai-internal.ksyun.com";

char ak[100];
char sk[100];

int load_key();
using namespace ks3;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("[Usage] %s [region(bj/gz/sh)] [bucket_name]\n", argv[0]);
        return 0;
    }
    int ret = load_key();
    if (ret != 0) {
        printf("[ERROR] load key failed\n");
        return ret;
    }

    char* region = argv[1];
    char* bucket = argv[2];
    std::string host;
    if (strncmp(region, "bj", strlen("bj")) == 0) {
        host = bj_host;
    } else if (strncmp(region, "gz", strlen("gz")) == 0) {
        host = gz_host;
    } else if (strncmp(region, "sh", strlen("sh")) == 0) {
        host = sh_host;
    } else {
        printf("[ERROR] unknown region %s\n", region);
        return 0;
    }

    sdk::KS3Client::InitGlobalCurl();

    sdk::KS3Client client(host);
    int code = client.Init();
    if (code != 0) {
        printf("failed to init client\n");
        return -1;
    }
    sdk::ClientContext ctx;
    ctx.bucket = bucket;
    ctx.accesskey = ak;
    ctx.secretkey = sk;

    // delete cross region replication
    sdk::KS3Response result_response;
    code = client.DeleteReplication(ctx, &result_response);
    if (code != 0) {
        printf("failed to call get cross region replication, curl with error code %d\n", code);
        return -1;
    }
    if (result_response.status_code == 200) {
        printf("[OK] get cross region replication ok\n");
    } else {
        printf("[ERROR] get cross region replication status_code=%d\n", result_response.status_code);
        printf("[ERROR] get cross region replication status_msg=%s\n", result_response.status_msg.c_str());
    }

    return 0;
}

int load_key() {
    // load_key
    const char* key_file = "/home/lihaibing/testkey";
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

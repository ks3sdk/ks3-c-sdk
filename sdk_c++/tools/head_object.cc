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
const char* hz_host = "kss-internal.ksyun.com";
const char* sh_host = "ks3-cn-shanghai-internal.ksyun.com";

char ak[100];
char sk[100];

int load_key();

using namespace ks3;
int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("[Usage] %s [region(bj/hz/sh)] [bucket_name] [object_key]\n", argv[0]);
        return 0;
    }
    int ret = load_key();
    if (ret != 0) {
        printf("[ERROR] load key failed\n");
        return ret;
    }

    char* region = argv[1];
    char* bucket = argv[2];
    char* object_key = argv[3];
    std::string host;
    if (strncmp(region, "bj", strlen("bj")) == 0) {
        host = bj_host;
    } else if (strncmp(region, "hz", strlen("hz")) == 0) {
        host = hz_host;
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
    ctx.object_key = object_key;
    ctx.accesskey = ak;
    ctx.secretkey = sk;
    sdk::KS3Response response;
    code = client.DeleteObject(ctx, &response);
    if (code != 0) {
        printf("failed to call curl with error code %d\n", code);
        return -1;
    }

    if (response.status_code == 200) {
        printf("[OK] delete file ok\n");
    } else {
        printf("[ERROR] status_code=%d\n", response.status_code);
        printf("[ERROR] status_msg=%s\n", response.status_msg.c_str());
    }
    // printf all headers
    std::map<std::string, std::string>::iterator it = response.res_headers.begin();
    for (; it != response.res_headers.end(); ++it) {
        std::string tmp(it->first);
        tmp.append(":");
        tmp.append(it->second);
        tmp.append("\n");
        printf("%s", tmp.c_str());
    }
    // printf content
    printf("%s\n", response.content.c_str());

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

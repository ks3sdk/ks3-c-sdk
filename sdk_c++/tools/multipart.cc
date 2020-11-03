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
    if (argc != 5) {
        printf("[Usage] %s [region(bj/gz/sh)] [bucket_name] [object_key] [content_string]\n", argv[0]);
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
    char* content = argv[4];
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
    ctx.object_key = object_key;
    ctx.accesskey = ak;
    ctx.secretkey = sk;
    sdk::KS3Response response;

    // 1. init multipart upload
    std::string uploadId;
    code = client.InitMultipartUpload(ctx, &response, &uploadId);
    if (code != 0) {
        printf("failed to call curl with error code %d\n", code);
        return -1;
    }

    if (response.status_code == 200) {
        printf("[OK] init multipart upload ok, upload id is %s\n", uploadId.c_str());
    } else {
        printf("[ERROR] init multipart failed status_code=%d, status_ms=%s\n", response.status_code,
               response.status_msg.c_str());
        return -1;
    }

    // 2. upload part
    ctx.partNum = 1;
    ctx.uploadId = uploadId;
    sdk::KS3Response res1;
    std::string etag;
    code = client.UploadPart(ctx, content, strlen(content), &res1, &etag);
    if (code != 0) {
        printf("failed to call curl with error code %d\n", code);
        return -1;
    }

    if (res1.status_code == 200) {
        printf("[OK] upload part 1 ok, etag is %s\n", etag.c_str());
    } else {
        printf("[ERROR] init multipart failed status_code=%d, status_ms=%s\n", res1.status_code,
               res1.status_msg.c_str());
        sdk::KS3Response res2;
        client.AbortMultipartUpload(ctx, &res2);
        return -1;
    }

    // 3. list parts;
    std::map<int, std::string> list_parts;
    sdk::KS3Response res7;
    code = client.ListParts(ctx, &list_parts, &res7);
    if (code != 0) {
        printf("failed to call curl with error code %d\n", code);
        return -1;
    }

    if (res7.status_code == 200) {
        printf("[OK] list list_parts ok\n");
    } else {
        printf("[ERROR] status_code=%d\n", res7.status_code);
        printf("[ERROR] status_msg=%s\n", res7.status_msg.c_str());
    }

    printf("list_parts info are \n");
    std::map<int, std::string>::iterator it = list_parts.begin();
    for (; it != list_parts.end(); ++it) {
        printf("%d:%s\n", it->first, it->second.c_str());
    }

    // 4. complete multipart
    std::map<int, std::string> parts;
    parts.insert(std::pair<int, std::string>(1, etag));
    sdk::KS3Response res3;
    code = client.CompleteMultipartUpload(ctx, parts, &res3);
    if (code != 0) {
        printf("failed to call curl with error code %d\n", code);
        return -1;
    }

    if (res3.status_code == 200) {
        printf("[OK] complete multipart upload part ok\n");
    } else {
        printf("[ERROR] init multipart failed status_code=%d, status_ms=%s\n", res3.status_code,
               res3.status_msg.c_str());
        sdk::KS3Response res2;
        client.AbortMultipartUpload(ctx, &res2);
        return -1;
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

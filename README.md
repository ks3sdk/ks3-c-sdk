# KS3 SDK For C/C++使用指南
---

##  注意

文档中的示例代码仅供参考之用，具体使用的时候请参考KS3 API文档，根据自己的实际情况调节参数。

##  1 概述

此SDK适用于Linux/Windows环境下C/C++版本。基于KS3 API 构建。

## 2 初始化

### 2.1 下载源码
https://github.com/ks3sdk/ks3-c-sdk

### 2.2 获取秘钥

1. 开通KS3服务，http://www.ksyun.com/user/register 注册账号
2. 进入控制台, http://ks3.ksyun.com/console.html#/setting 获取AccessKeyID 、AccessKeySecret

## 3 快速入门

请先阅读常用概念术语文档, http://ks3.ksyun.com/doc/api/index.html

常见示例请参考源码中example.c文件

###	3.1 创建一个bucket

```
const char* host = "kss.ksyun.com";
const char* bucket = "YOUR_BUCKET";
    const char* ak = "YOUR_ACCESS_KEY";
    const char* sk = "YOUR_SECRET_KEY";

int error;
buffer* resp = NULL;

resp = create_bucket(host, bucket, ak, sk, NULL, &error);
if (error != 0) {
    printf("curl err=%d\n", error);
} else {
    printf("status code=%d\n", resp->status_code);
    printf("status msg=%s\n", resp->status_msg);
    if (resp->body != NULL) {
        printf("error msg=%s\n", resp->body);
    }
}
buffer_free(resp);
```

###	3.2 删除一个bucket

```
const char* host = "kss.ksyun.com";
const char* bucket = "YOUR_BUCKET";
    const char* ak = "YOUR_ACCESS_KEY";
    const char* sk = "YOUR_SECRET_KEY";

int error;
buffer* resp = NULL;

resp = delete_bucket(host, bucket, ak, sk, NULL, &error);
if (error != 0) {
    printf("curl err=%d\n", error);
} else {
    printf("status code=%d\n", resp->status_code);
    printf("status msg=%s\n", resp->status_msg);
    if (resp->body != NULL) {
        printf("error msg=%s\n", resp->body);
    }
}
buffer_free(resp);
```

### 3.3 列出用户所有桶

```
const char* host = "kss.ksyun.com";
const char* ak = "YOUR_ACCESS_KEY";
const char* sk = "YOUR_SECRET_KEY";

int error;
buffer* resp = NULL;

resp = list_all_bucket(host, ak, sk, &error);
if (error != 0) {
    printf("curl err=%d\n", error);
} else {
    printf("status code=%d\n", resp->status_code);
    printf("status msg=%s\n", resp->status_msg);
    if (resp->body != NULL) {
        printf("error msg=%s\n", resp->body);
    }
}
buffer_free(resp);
```


### 3.4 上传文件

```
const char* host = "kss.ksyun.com";
const char* bucket = "YOUR_BUCKET";
const char* object_key = "YOUR_OBJECT_KEY";
const char* filename = "LOCAL_DISK_FILE_PATH";
const char* ak = "YOUR_ACCESS_KEY";
const char* sk = "YOUR_SECRET_KEY";

int error;
buffer* resp = NULL;

resp = upload_file_object(host, bucket,
    object_key, filename, ak, sk, NULL, NULL, &error);
if (error != 0) {
    printf("curl err=%d\n", error);
} else {
    printf("status code=%d\n", resp->status_code);
    printf("status msg=%s\n", resp->status_msg);
    if (resp->body != NULL) {
        printf("error msg=%s\n", resp->body);
    }
}
buffer_free(resp);
```

### 3.5 带header上传文件

**注意：header之间要以'\n'分隔**

ACL 特殊头部
用户可以通过以下的header为Object设置预设的ACL

名称	描述	必须
x-kss-acl	用于对象的预定义权限。
类型：String
默认值：private
有效值：private &##124; public-read
约束条件：无	否
如果用户期望为Bucket设置详细的ACL，可以通过以下header设置

名称	描述	必须
x-kss-grant-read	为若干用户授予READ权限。
类型：String
默认值：无
约束条件：无	否
x-kss-grant-write	为若干用户授予WRITE权限。
类型：String
默认值：无
约束条件：无	否
x-kss-grant-full-control	为若干用户授予FULL_CONTROL权限。
类型：String
默认值：无
约束条件：无
```
const char* headers = "x-kss-acl:public-read\nx-kss-callbackurl:http://www.callbackurl.com/";
resp = upload_file_object(host, bucket,
    object_key, filename, ak, sk, NULL, headers, &error);
if (error != 0) {
    printf("curl err=%d\n", error);
} else {
    printf("status code=%d\n", resp->status_code);
    printf("status msg=%s\n", resp->status_msg);
    if (resp->body != NULL) {
        printf("error msg=%s\n", resp->body);
    }
}
buffer_free(resp);
```


### 3.6 下载文件
    const char* to_save_file_name = "./local_save";
    resp = download_file_object(host, bucket,
        object_key, to_save_file_name, ak, sk, NULL, &error);
    if (error != 0) {
        printf("curl err=%d\n", error);
    } else {
        printf("status code=%d\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        if (resp->body != NULL) {
            printf("error msg=%s\n", resp->body);
        }
    }
    buffer_free(resp);

### 3.7 删除文件

```
const char* object_key = "YOUR_OBJECT_KEY";
resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
if (error != 0) {
    printf("curl err=%d\n", error);
} else {
    printf("status code=%d\n", resp->status_code);
    printf("status msg=%s\n", resp->status_msg);
    if (resp->body != NULL) {
        printf("error msg=%s\n", resp->body);
    }
}
buffer_free(resp);
```

### 3.8 复制文件

```
const char* src_bucket = "SRC_BUCKET_NAME";
const char* src_object_key = "SRC_OBJECT_KEY";
const char* dst_bucket = "DST_BUCKET";
const char* dst_object_key = "DST_OBJECT_KEY";

resp = copy_object(host, src_bucket, src_object_key,
        dst_bucket, dst_object_key, ak, sk, NULL, NULL, &error);

if (error != 0) {
    printf("curl err=%d\n", error);
} else {
    printf("status code=%d\n", resp->status_code);
    printf("status msg=%s\n", resp->status_msg);
    if (resp->body != NULL) {
        printf("error msg=%s\n", resp->body);
    }
}
buffer_free(resp);
```

### 3.9 上传buf object

```
const char* buf = "hello world";
int buf_len = strlen(buf);

resp = upload_object(host, bucket,
    object_key, buf, buf_len, ak, sk, NULL, NULL, &error);
if (error != 0) {
    printf("curl err=%d\n", error);
} else {
    printf("status code=%d\n", resp->status_code);
    printf("status msg=%s\n", resp->status_msg);
    if (resp->body != NULL) {
        printf("error msg=%s\n", resp->body);
    }
}
free(buf);
buffer_free(resp);
```




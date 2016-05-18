
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"

int load_key();
void api_test_bucket_relative();
void api_test_object_relative();
void api_test_buf_relative();

char ak[100] = { '\0' };
char sk[100] = { '\0' };

int main()
{
    int ret = load_key();
    if (ret != 0) {
        return -1;
    }
	api_test_bucket_relative();
	api_test_object_relative();
	api_test_buf_relative();
	
	//system("pause");
	return 0;
}

int load_key() {
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
    sk[len - 1] = '\0';
    printf("sk=%s\n", sk);
    fclose(fp);
    return 0;
}

void api_test_bucket_relative() {
	char* bucket = "win-c-bucket1";	
	const char* host = "kss.ksyun.com"; // hz
	//const char* host = "ks3-cn-beijing.ksyun.com"; // bj host
    //const char* host = "ks3-us-west-1.ksyun.com"; // american host
    //const char* host = "ks3-cn-hk-1.ksyun.com"; // hk host 

	buffer* resp;
	// curl错误码
	int error;

	//列举当前账户下所有bucket,结果在resp
	printf("before list_all_bucket\n");
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
	printf("after list_all_bucket\n");

 	//新建一个bucket,如果失败，错误原因xml保存在resp
	printf("before create_bucket\n");
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
	printf("after create_bucket\n");

	printf("before create_bucket\n");
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
	printf("after create_bucket\n");

    //删除一个bucket，如果失败，错误原因xml保存在resp
	printf("before delete_bucket\n");
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
	printf("after delete_bucket\n");

	printf("before delete_bucket\n");
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
	printf("after delete_bucket\n");
	
}

void api_test_object_relative() {
	char* bucket = "c-bucket1";
	char* filename = "/home/hanbing1/files/tmp";
	char* save = "/home/hanbing1/files/save_download_file";
	const char* host = "kss.ksyun.com"; // hz
	//"location=adsf&lifecycle=qwe&acl=123&website=123&website=435";
	char* query_args = "";
	char* headers = "x-kss-callbackurl:http://10.4.2.38:19090/\ncontent-type:text";
	
	// 以下示例是各个接口调用	
	// 服务端响应内容保存在动态buffer
	// buffer使用完，需要调用buffer_free函数释放内存
	buffer* resp;
	// curl错误码
	int error;

	//上传一个文件到指定的bucket下面
	printf("before upload file object\n");
	resp = upload_file_object(host, bucket,
		"win32/xxx1.log", filename, ak, sk, query_args, NULL, &error);
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
	printf("after upload file object\n");
	
	printf("before upload file object\n");
	resp = upload_file_object(host, bucket,
		"win32/xxx2.log", filename, ak, sk, query_args, NULL, &error);
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
	printf("after upload file object\n");
	
	printf("before upload file object with query_args and headers\n");
	resp = upload_file_object(host, bucket,
		"win32/xxx3.log", filename, ak, sk, query_args, headers, &error);
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
	printf("after upload file object with query_args and headers\n");

	//从bucket下载一个object保存在本地文件
	printf("before download file object\n");
	resp = download_file_object(host, bucket,
		"win32/xxx1.log", save, ak, sk, NULL, &error);
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
	printf("after download file object\n");

	//从bucket删除一个指定的object，如果失败，错误原因xml保存在resp
	printf("before delete file object\n");
	resp = delete_object(host, bucket,
		"win32/xxx1.log", ak, sk, NULL, &error);
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
	printf("after delete file object\n");

	printf("before delete file object\n");
	resp = delete_object(host, bucket,
		"win32/xxx1.log", ak, sk, NULL, &error);
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
	printf("after delete file object\n");

	//列举指定bucket下所有object，结果在resp
	printf("before list object\n");
	resp = list_bucket_objects(host, bucket,
		ak, sk, NULL, &error);
	if (error != 0) {
		printf("curl err=%d\n", error);
	} else {
		printf("status code=%d\n", resp->status_code);
		printf("status msg=%s\n", resp->status_msg);
		//if (resp->body != NULL) {
			//printf("error msg=%s\n", resp->body);
		//}
	}
	buffer_free(resp);
	printf("after list object\n");
	
}

void api_test_buf_relative() {
	char* host = "kss.ksyun.com"; // hz
	char* bucket = "c-bucket1";
    char* filename = "/home/hanbing1/files/tmp";
    char* save = "/home/hanbing1/files/save_download_buf";
	FILE *stream;
	char* buf = (char*) malloc (13033174);
    int error;
    char* object_key = "linux/buf_object_file3";
    buffer* resp = NULL;
    int flen = 13033174;

	printf("\nbefore upload buf object by file\n");
	if ((stream = fopen(filename, "r"))	== NULL) {
		fprintf(stderr,
				"Cannot open output file.\n");
		return;
	}
	size_t rlen = fread(buf, 1, flen, stream);
	printf("rlen=%d\n", rlen);
	fclose(stream);
	printf("ak=%s, sk=%s\n", ak, sk);

	resp = upload_object(host, bucket,
		object_key, buf, flen, ak, sk, NULL, NULL, &error);
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

	printf("before download file object\n");
	resp = download_file_object(host, bucket,
		object_key, save, ak, sk, NULL, &error);
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
}



#include <stdio.h>
#include <stdlib.h>
#include "api.h"

void api_test_bucket_relative();
void api_test_object_relative();

int main()
{
	api_test_bucket_relative();
	api_test_object_relative();
	
	system("pause");
	return 0;
}

void api_test_bucket_relative() {
	char* id = "S1guCl0KF/oA285zzEDK";
    char* key = "DGSTgVMQ08EepL3CanUoatVV9en7mB856ljbNEaK";
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
	resp = list_all_bucket(host, id, key, &error);
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
	resp = create_bucket(host, bucket, id, key, NULL, &error);
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
	resp = create_bucket(host, bucket, id, key, NULL, &error);
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
	resp = delete_bucket(host, bucket, id, key, NULL, &error);
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
	resp = delete_bucket(host, bucket, id, key, NULL, &error);
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
	char* id = "S1guCl0KF/oA285zzEDK";
    char* key = "DGSTgVMQ08EepL3CanUoatVV9en7mB856ljbNEaK";
	char* filename = "./localfile";
	char* save = "./test_save.log";
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
	resp = upload_file_object(host, "c-bucket1",
		"win32/xxx1.log", filename, id, key, query_args, NULL, &error);
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
	resp = upload_file_object(host, "c-bucket1",
		"win32/xxx2.log", filename, id, key, query_args, NULL, &error);
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
	resp = upload_file_object(host, "c-bucket1",
		"win32/xxx3.log", filename, id, key, query_args, headers, &error);
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
	resp = download_file_object(host, "c-bucket1",
		"win32/xxx1.log", save, id, key, NULL, &error);
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
	resp = delete_object(host, "c-bucket1",
		"win32/xxx1.log", id, key, NULL, &error);
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
	resp = delete_object(host, "c-bucket1",
		"win32/xxx1.log", id, key, NULL, &error);
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
	resp = list_bucket_objects(host, "c-bucket1",
		id, key, NULL, &error);
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

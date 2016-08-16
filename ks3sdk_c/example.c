
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "api.h"
#include "example.h"

int load_key();
void api_test_bucket_relative();
void api_test_object_relative();
void api_test_buf_relative();
void api_test_copy_object();
void api_test_download_object_to_buffer();
void api_test_init_upload_complete();
void api_test_init_upload_listparts_complete();
void api_test_init_listmultiparts_abort();

char ak[100] = {0};
char sk[100] = {0};
char host[512] = {0};
const char* bucket = "bucket-test-for-multiparts-100";

int main()
{
    int ret = load_key();
    if (ret != 0) {
        return -1;
    }
    ks3_global_init();
    /*
	api_test_bucket_relative();
	api_test_object_relative();
	api_test_buf_relative();
	api_test_copy_object();
	api_test_download_object_to_buffer();
	*/
    ret = CreateBucket(host, bucket, ak, sk); 
    
    api_test_init_upload_complete();
    api_test_init_upload_listparts_complete();
    api_test_init_listmultiparts_abort();
   
    ret = DeleteBucket(host, bucket, ak, sk);
    ks3_global_destroy();
	
	//system("pause");
	return 0;
}

int load_key() {
    char* key_file = "/tmp/key_test_sdk";
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
    
    fgets(host, 512, fp);
    len = strlen(host);
    host[len - 1] = '\0';
    printf("host=%s\n", host );
    
    fclose(fp);
    return 0;
}

void api_test_bucket_relative() {
	char* bucket = "win-c-bucket1";	

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
		"win32/xxx1.log", save, ak, sk, NULL, NULL, &error);
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
		object_key, save, ak, sk, NULL, NULL, &error);
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

void api_test_copy_object() {
    int error;
    buffer* resp = NULL;

    char* host = "kss.ksyun.com";
    char* src_bucket = "c-bucket1";
    char* src_object_key = "4.log";
    char* dst_bucket = "c-bucket1";
    char* dst_object_key = "4.log-dst";
    resp = copy_object(host, src_bucket, src_object_key,
            dst_bucket, dst_object_key, ak, sk, NULL, NULL, &error);
    printf("copy_object curl error=%d\n", error);
    printf("copy_object status code=%d\n", resp->status_code);
    printf("copy_object status msg=%s\n", resp->status_msg);
    printf("copy_object err msg=%s\n", resp->body);

    // free memory
    buffer_free(resp);
}

void api_test_download_object_to_buffer() {
    int error;
    buffer* resp = NULL;

    char* host = "kss.ksyun.com";
    char* bucket = "c-bucket1";
    char* object_key = "file1";
    char* save_file = "./download_obj_save";
    resp = download_object(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    printf("download_object curl error=%d\n", error);
    printf("download_object status code=%d\n", resp->status_code);
    printf("download_object status msg=%s\n", resp->status_msg);
    printf("download_object err msg=%s\n", resp->body);
    if (resp->content_length > 0 &&
            (resp->status_code == 200 || resp->status_code == 206)) {
        printf("object content_length=%d\n", resp->content_length);
        // save file
        int fd = open(save_file, O_CREAT | O_RDWR);
        int ret;
        assert(fd > 0);
        ret = pwrite(fd, resp->content, resp->content_length, 0);
        assert(ret == resp->content_length);
        close(fd);
    }

    // free memory
    buffer_free(resp);
}

void api_test_init_upload_complete() {
    int error;
    const char* filename = "./lib/libcurl_mtd.lib";

    buffer *resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char uploadid_str[128] = { 0 };

    strcpy(query_str, "uploads");
    strcpy(object_key, "test_multiparts_object_01");

    // 1. init_multipart
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    if (0 == error && 200 != resp->status_code) {
        printf("create_bucket:\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    }

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
        buffer_free(resp);
        
        snprintf(query_str, 1024, "uploadId=%s", uploadid_str);
        resp = abort_multipart_upload(host, bucket, object_key, ak, sk, query_str, NULL, &error);
        if (resp->status_code != 204) {
            printf("test abort_multipart_upload:\n");
            printf("status code = %ld\n", resp->status_code);
            printf("status msg = %s\n", resp->status_msg);
            printf("error msg = %s\n", resp->body);
            return;
        }
        goto END_TAG;
    }
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
END_TAG:
    buffer_free(resp);

    if (buf)
        free(buf);
    buf = NULL;
    if (part_result_arr)
        free(part_result_arr);
    part_result_arr = NULL;


    return;
}
void api_test_init_upload_listparts_complete(){
    int error;
    const char* filename = "./lib/libcurl_mtd.lib";

    buffer *resp = NULL;
    char object_key[1024];
    char query_str[1024];
    char header_str[1024];
    char objectid_str[128] = { 0 };

    strcpy(object_key, "test_multiparts_object_00");

    // 1. init_multipart
    resp = init_multipart_upload(host, bucket, object_key, ak, sk, NULL, NULL, &error);
    if (200 != resp->status_code) {
        printf("create_bucket:\nstatus code=%ld\n", resp->status_code);
        printf("status msg=%s\n", resp->status_msg);
        printf("error msg=%s\n", resp->body);
    }

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

    // 5. complete_multipart_upload
    int buf_size = 5 * 1024 * 1024;
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
    }
    buffer_free(resp);

    resp = delete_object(host, bucket, object_key, ak, sk, NULL, &error);
    if (resp->status_code != 204) {
        printf("test delete_object:\n");
        printf("status code = %ld\n", resp->status_code);
        printf("status msg = %s\n", resp->status_msg);
        printf("error msg = %s\n", resp->body);
    }
    buffer_free(resp);
    free(buf);
    return;
}
void api_test_init_listmultiparts_abort(){

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
        buffer_free(resp);
    }

    free(buf);
    free(off_arr);
    return;
}

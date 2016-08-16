
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curl/curl.h"
#include "api.h"
#include "make_header.h"
#include "buffer.h"

int ks3_global_init() {
   return curl_global_init(CURL_GLOBAL_ALL); 
}
void ks3_global_destroy() {
    curl_global_cleanup();
}


buffer* list_all_bucket(const char* host, const char* access_key,
	const char* secret_key, int* err) {
	buffer* resp = NULL;
	resp = buffer_init();
	make_header(host, GET_METHOD, NULL, NULL, NULL, NULL, NULL,
		access_key, secret_key, resp, err);
	return resp;
}

buffer* list_bucket_objects(const char* host,
	const char* bucket, const char* access_key,
	const char* secret_key, const char* query_args, int* err) {
	buffer* resp = NULL;
	resp = buffer_init();
	make_header(host, GET_METHOD, bucket, NULL, NULL, query_args, NULL,
		access_key, secret_key, resp, err);
	return resp;
}

buffer* create_bucket(const char* host,
	const char* bucket, const char* access_key,
	const char* secret_key, const char* query_args, int* err) {
	buffer* resp = NULL;
	resp = buffer_init();
	make_header(host, PUT_METHOD, bucket, NULL, NULL, query_args, NULL,
		access_key, secret_key, resp, err);
	return resp;
}

buffer* delete_bucket(const char* host,
	const char* bucket, const char* access_key,
	const char* secret_key, const char* query_args, int* err) {
	buffer* resp = NULL;
	resp = buffer_init();
	make_header(host, DELETE_METHOD, bucket, NULL, NULL, query_args, NULL,
		access_key, secret_key, resp, err);
	return resp;
}

buffer* upload_file_object(const char* host,
	const char* bucket, const char* object_key,
	const char* filename, const char* access_key,
	const char* secret_key, const char* query_args,
	const char* headers, int* err) {
	buffer* resp = NULL;
	resp = buffer_init();
	make_header_file(host, PUT_METHOD, bucket, object_key, filename,
		query_args, headers, access_key, secret_key, resp, err);
	return resp;
}

buffer* download_file_object(const char* host,
	const char* bucket, const char* object_key, const char* filename,
	const char* access_key, const char* secret_key, const char* query_args,
	const char* headers, int* err) {
	buffer* resp = NULL;
	resp = buffer_init();
	make_header_file(host, GET_METHOD, bucket, object_key, filename,
		query_args, headers, access_key, secret_key, resp, err);
	return resp;
}

buffer* upload_object(const char* host, const char* bucket,
    const char* object_key, const char* buf_data, int buf_len,
    const char* access_key, const char* secret_key,
    const char* query_args, const char* headers, int* err) {
    buffer* resp = NULL;
    resp = buffer_init();
    make_header_buf(host, PUT_METHOD, bucket, object_key, buf_data,
            buf_len, query_args, headers, access_key, secret_key, resp, err);
    return resp;
}

extern buffer* download_object(const char* host, const char* bucket,
    const char* object_key, const char* access_key, const char* secret_key,
    const char* query_args, const char* headers, int* err) {
    buffer* resp = NULL;
    resp = buffer_init();
    make_header_buf(host, GET_METHOD, bucket, object_key, NULL,
            -1, query_args, headers, access_key, secret_key, resp, err);
    return resp;
}

buffer* delete_object(const char* host,
	const char* bucket, const char* object_key,
	const char* access_key, const char* secret_key,
	const char* query_args, int* err) {
	buffer* resp = NULL;
	resp = buffer_init();
	make_header(host, DELETE_METHOD, bucket, object_key, NULL,
		query_args, NULL, access_key, secret_key, resp, err);
	return resp;
}

buffer* copy_object(const char* host, const char* src_bucket,
   const char* src_object_key, const char* dst_bucket,
   const char* dst_object_key, const char* access_key,
   const char* secret_key, const char* query_args,
   const char* headers, int* err) {
	buffer* resp = NULL;
	resp = buffer_init();
	char actual_header[1024] = { '\0' };
	if (headers != NULL) {
	    strcat(actual_header, headers);
	    strcat(actual_header, "\n");
	}
	char src_header[100] = { '\0' };
	strcat(src_header, "x-kss-copy-source: /");
	if (src_bucket != NULL) {
	    strcat(src_header, src_bucket);
	    strcat(src_header, "/");
	}
	if (src_object_key != NULL) {
	    strcat(src_header, src_object_key);
	}
	strcat(actual_header, src_header);
	make_header(host, PUT_METHOD, dst_bucket, dst_object_key,
	        NULL, query_args, actual_header, access_key, secret_key, resp, err);
	return resp;
}

buffer* init_multipart_upload(const char* host,
    const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key,
    const char* query_args, const char* headers, int* err) {
    buffer* resp = NULL;
    char query_buf[1024];
    
    if (query_args && strlen(query_args) > 0) 
        snprintf(query_buf, 1024, "uploads&%s", query_args);
    else
        snprintf(query_buf, 1024, "uploads");

    resp = buffer_init();
    make_multiparts(host, POST_METHOD, bucket, object_key, NULL, 0, query_buf, 
        headers, MULTI_OP, access_key, secret_key, resp, err);
    return resp;
}

buffer* upload_part(const char* host, const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key,
    const char* buf_data, int buf_len,
    const char* query_args, const char* headers, int* err) {
    buffer* resp = NULL;
    resp = buffer_init();
    make_header_buf(host, PUT_METHOD, bucket, object_key, buf_data, buf_len, 
        query_args, headers, access_key, secret_key, resp, err);
    return resp;
}

buffer* complete_multipart_upload(const char* host,
    const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key,
    const char* buf_data, int buf_len,
    const char* query_args, const char* headers, int* err) {
    buffer* resp = NULL;
    resp = buffer_init();
    make_multiparts(host, POST_METHOD, bucket, object_key, buf_data, buf_len, query_args, 
        headers, MULTI_COMPLETE_OP, access_key, secret_key, resp, err);
    return resp;
}

buffer* abort_multipart_upload(const char* host,
    const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key,
    const char* query_args, const char* headers, int* err) {
    buffer* resp = NULL;
    resp = buffer_init();
    make_header(host, DELETE_METHOD, bucket, object_key, NULL, query_args, 
        headers, access_key, secret_key, resp, err);
    return resp;
}

buffer* list_multipart_uploads(const char* host, const char* bucket,
    const char* access_key, const char* secret_key,
    const char* query_args, const char* headers, int* err) {
    buffer* resp = NULL;
    char query_buf[1024];
    
    if (query_args && strlen(query_args) > 0) 
        snprintf(query_buf, 1024, "uploads&%s", query_args);
    else
        snprintf(query_buf, 1024, "uploads");
    
    resp = buffer_init();
    make_header(host, GET_METHOD, bucket, NULL, NULL, query_buf, headers, 
        access_key, secret_key, resp, err);
    return resp;
}

buffer* list_parts(const char* host,
    const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key,
    const char* query_args, const char* headers, int* err) {
    buffer* resp = NULL;
    resp = buffer_init();
    make_header(host, GET_METHOD, bucket, object_key, NULL, query_args, headers, 
        access_key, secret_key, resp, err);
    return resp;
}


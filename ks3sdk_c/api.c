
#include <stdio.h>
#include <stdlib.h>
#include "api.h"
#include "make_header.h"
#include "buffer.h"

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
	const char* bucket, const char* object_key,
	const char* filename, const char* access_key,
	const char* secret_key, const char* query_args, int* err) {
	buffer* resp = NULL;
	resp = buffer_init();
	make_header_file(host, GET_METHOD, bucket, object_key, filename,
		query_args, NULL, access_key, secret_key, resp, err);
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
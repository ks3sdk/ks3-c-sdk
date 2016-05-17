
#ifndef _KS3_SDK_C_API_H_
#define _KS3_SDK_C_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "buffer.h"

extern buffer* list_all_bucket(const char* host,
	const char* access_key, const char* secret_key, int* err);
extern buffer* list_bucket_objects(const char* host,
	const char* bucket, const char* access_key,
	const char* secret_key, const char* query_args, int* err);

extern buffer* create_bucket(const char* host,
	const char* bucket, const char* access_key,
	const char* secret_key, const char* query_args, int* err);
extern buffer* delete_bucket(const char* host,
	const char* bucket, const char* access_key,
	const char* secret_key, const char* query_args, int* err);

extern buffer* upload_file_object(const char* host, const char* bucket,
	const char* object_key, const char* filename,
	const char* access_key, const char* secret_key,
	const char* query_args, const char* headers, int* err);
extern buffer* download_file_object(const char* host,
	const char* bucket, const char*object_key,
	const char* filename, const char* access_key,
	const char* secret_key, const char* query_args, int* err);
extern buffer* upload_buf_object(const char* host, const char* bucket,
	const char* object_key, const char* buf_content,
	const char* access_key, const char* secret_key,
	const char* query_args, const char* headers, int* err);

extern buffer* delete_object(const char* host,
	const char* bucket, const char* object_key,
	const char* access_key, const char* secret_key,
	const char* query_args, int* err);

#ifdef __cplusplus
}
#endif
#endif  // _KS3_SDK_C_API_H_


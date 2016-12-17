
#ifndef _KS3_SDK_C_API_H_
#define _KS3_SDK_C_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "buffer.h"

extern int  ks3_global_init();
extern void ks3_global_destroy();

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
	const char* bucket, const char* object_key, const char* filename,
	const char* access_key, const char* secret_key,
	const char* query_args, const char* headers, int* err);
extern buffer* upload_object(const char* host, const char* bucket,
    const char* object_key, const char* buf_data, int buf_len,
    const char* access_key, const char* secret_key,
    const char* query_args, const char* headers, int* err);
extern buffer* download_object(const char* host, const char* bucket,
    const char* object_key, const char* access_key, const char* secret_key,
    const char* query_args, const char* headers, int* err);

extern buffer* head_object(const char* host,
	const char* bucket, const char* object_key,
	const char* access_key, const char* secret_key,
	const char* query_args, int* err);

extern buffer* delete_object(const char* host,
	const char* bucket, const char* object_key,
	const char* access_key, const char* secret_key,
	const char* query_args, int* err);

extern buffer* copy_object(const char* host, const char* src_bucket,
    const char* src_object_key, const char* dst_bucket,
    const char* dst_object_key, const char* access_key,
    const char* secret_key, const char* query_args,
    const char* headers, int* err);

extern buffer* init_multipart_upload(const char* host, 
    const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key, 
    const char* query_args, const char* headers, int* err);

extern buffer* upload_part(const char* host,
    const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key, 
    const char* upload_id, int part_number,
    const char* part_data, int part_data_len,
    const char* query_args, const char* headers, int* err);

extern buffer* complete_multipart_upload(const char* host, 
    const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key, 
    const char* upload_id, const char* parts_info, int parts_info_len,
    const char* query_args, const char* headers, int* err);

extern buffer* abort_multipart_upload(const char* host, 
    const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key, 
    const char* upload_id, const char* query_args,
    const char* headers, int* err);

extern buffer* list_multipart_uploads(const char* host,
    const char* bucket, const char* access_key, const char* secret_key, 
    const char* query_args, const char* headers, int* err);

extern buffer* list_parts(const char* host, 
    const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key,
    const char* upload_id, const char* query_args,
    const char* headers, int* err);

#ifdef __cplusplus
}
#endif
#endif  // _KS3_SDK_C_API_H_


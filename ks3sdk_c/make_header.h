
#ifndef _KS3_SDK_C_MAKE_HEADER_H_
#define _KS3_SDK_C_MAKE_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "buffer.h"

typedef enum {
    META_OP = 0,
    FILE_OP,
    BUF_OP,
    MULTI_OP,
    MULTI_COMPLETE_OP,
    OpType_END
} OpType;

typedef enum {
    GET_METHOD = 0,
	PUT_METHOD,
	DELETE_METHOD,
    POST_METHOD,
    HEAD_METHOD,
    MethodType_END
} MethodType;

static void set_method(MethodType method_type, void* handler);
static char* compute(const char* sign, const char* secret_key, char* b64);
static char* make_url(const char* host, const char* bucket_name,
	const char* object, const char* query_args, char* url);
static int make_header_common(const char* host, MethodType method_type,
	const char* bucket, const char* object, const char* data, int buf_len,
	const char* query_args, const char* headers, OpType op_type,
	const char* access_key, const char* secret_key, buffer* resp);

extern void make_header(const char* host, MethodType method_type,
	const char* bucket, const char* object, const char* filename,
	const char* query_args, const char* headers, const char* access_key,
	const char* secret_key, buffer* resp, int* err);
extern void make_header_file(const char* host, MethodType method_type,
	const char* bucket, const char* object, const char* filename,
	const char* query_args, const char* headers, const char* access_key,
	const char* secret_key, buffer* resp, int* err);
extern void make_header_buf(const char* host, MethodType method_type,
    const char* bucket, const char* object, const char* content, int buf_len,
    const char* query_args, const char* headers, const char* access_key,
    const char* secret_key, buffer* resp, int* err);
extern void make_multiparts(const char* host, MethodType method_type, 
    const char* bucket, const char* object, const char* buf_data, int buf_len, 
    const char* query_args, const char* headers, OpType op_type, const char* access_key,
    const char* secret_key, buffer* resp, int* err);

#ifdef __cplusplus
}
#endif
#endif  // _KS3_SDK_C_MAKE_HEADER_H_

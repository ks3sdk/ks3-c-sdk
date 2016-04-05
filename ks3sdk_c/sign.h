
#ifndef _KS3_SDK_C_SIGN_H_
#define _KS3_SDK_C_SIGN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "rb_tree.h"

static char * trim(char * src);
static int exist(const char** src,int num, const char* target);
static void split_kv(const char* src, const char* delim, key_value* kv);
static void build_query_kv_map(const char* query_args, rb_node_t** root);
static void encode_kv_map(rb_node_t* root, int need_code, char* out_str);
static void build_header_kv_map(const char* headers, rb_node_t** root);
static void canon_headers(const rb_node_t* root, char* out_headers);
static void canon_resource(const char* bucket, const char* obj,
	const char* query_args, char* res);

extern char* make_resource_quote(const char* query_args, char* out_str);
extern char* make_origin_sign(int method_type,
	const char* bucket, const char* obj,
	const char* query_args, const char* headers,                                                       const char* time, char* origin_sign);
extern char* get_time(char* out_time);

#ifdef __cplusplus
}
#endif
#endif  // _KS3_SDK_C_SIGN_H_

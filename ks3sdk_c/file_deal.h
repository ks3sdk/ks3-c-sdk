
#ifndef _KS3_SDK_C_FILE_DEAL_H_
#define _KS3_SDK_C_FILE_DEAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include "curl/curl.h"
#include "buffer.h"

typedef struct {
	const char* data;
	int32_t offset;
	int32_t len;
} BufData;

static size_t read_s3_string_data(void *ptr, size_t size,
        size_t nmemb, void *stream);
static size_t read_s3_response(void *ptr, size_t size,
        size_t nmemb, void *stream);
static size_t read_http_header_resp(void *ptr, size_t size,
        size_t nmemb, void *stream);

static void meta_deal_up(void *handler, void* ptr, size_t size);
static void meta_deal_down(void *handler, buffer* resp);
extern void meta_deal(void *handler, void* ptr, buffer* resp, size_t size);

static void file_deal_up(void *handler, FILE* file, curl_off_t size, buffer* resp);
static void file_deal_down(void *handler, FILE* file, buffer* resp);
extern void file_down(void *handler, FILE* file, buffer* resp);
extern void file_up(void *handler, FILE* file, curl_off_t size, buffer* resp);
extern void buf_up(void *handler, BufData* buf_data, curl_off_t size, buffer* resp);

#ifdef __cplusplus
}
#endif
#endif  // _KS3_SDK_C_FILE_DEAL_H_

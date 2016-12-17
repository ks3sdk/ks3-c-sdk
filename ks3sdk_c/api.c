
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

static int check_params(const char* access_key, const char* secret_key, int* err) {
    if (err == NULL) {
        fprintf(stderr, "pointer error is null\n");
        return -1;
    }
    if (access_key == NULL || secret_key == NULL) {
        fprintf(stderr, "access_key or secret_key is null\n");
        *err = -1;
        return -1;
    }
    return 0;
}

static int check_buf(const char * buf_data, int buf_len) {
    if (NULL == buf_data || buf_len <= 0) {
        fprintf(stderr, "buf_data is NULL or buf_len less then zero\n");
        return -1;
    }
    return 0;
}

buffer* list_all_bucket(const char* host, const char* access_key,
	const char* secret_key, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
	buffer* resp = NULL;
	resp = buffer_init();
	make_header(host, GET_METHOD, NULL, NULL, NULL, NULL, NULL,
		access_key, secret_key, resp, err);
	return resp;
}

buffer* list_bucket_objects(const char* host,
	const char* bucket, const char* access_key,
	const char* secret_key, const char* query_args, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
	buffer* resp = NULL;
	resp = buffer_init();
	make_header(host, GET_METHOD, bucket, NULL, NULL, query_args, NULL,
		access_key, secret_key, resp, err);
	return resp;
}

buffer* create_bucket(const char* host,
	const char* bucket, const char* access_key,
	const char* secret_key, const char* query_args, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
	buffer* resp = NULL;
	resp = buffer_init();
	make_header(host, PUT_METHOD, bucket, NULL, NULL, query_args, NULL,
		access_key, secret_key, resp, err);
	return resp;
}

buffer* delete_bucket(const char* host,
	const char* bucket, const char* access_key,
	const char* secret_key, const char* query_args, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
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
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
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
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
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
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
    if (check_buf(buf_data, buf_len) != 0) {
        *err = -1;
        return NULL;
    }
    buffer* resp = NULL;
    resp = buffer_init();
    make_header_buf(host, PUT_METHOD, bucket, object_key, buf_data,
            buf_len, query_args, headers, access_key, secret_key, resp, err);
    return resp;
}

extern buffer* download_object(const char* host, const char* bucket,
    const char* object_key, const char* access_key, const char* secret_key,
    const char* query_args, const char* headers, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
    buffer* resp = NULL;
    resp = buffer_init();
    make_header_buf(host, GET_METHOD, bucket, object_key, NULL,
            -1, query_args, headers, access_key, secret_key, resp, err);
    return resp;
}

buffer* head_object(const char* host,
	const char* bucket, const char* object_key,
	const char* access_key, const char* secret_key,
	const char* query_args, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
	buffer* resp = NULL;
	resp = buffer_init();
	make_header(host, HEAD_METHOD, bucket, object_key, NULL,
		query_args, NULL, access_key, secret_key, resp, err);
	return resp;
}

buffer* delete_object(const char* host,
	const char* bucket, const char* object_key,
	const char* access_key, const char* secret_key,
	const char* query_args, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
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
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
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
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
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

buffer* upload_part(const char* host, const char* bucket,
    const char* object_key, const char* access_key, const char* secret_key,
    const char* upload_id, int part_number, const char* part_data,
    int part_data_len, const char* query_args, const char* headers, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
    if (check_buf(part_data, part_data_len) != 0) {
        *err = -1;
        return NULL;
    }
    if (upload_id == NULL || strlen(upload_id) <= 0) {
        *err = -1;
        fprintf(stderr, "upload_id is NULL\n");
        return NULL;
    }
    if (part_number < 1 || part_number > 10000) {
        *err = -1;
        fprintf(stderr, "part_number %d not in range [1-10000]\n", part_number);
        return NULL;
    }
    char internal_query[1024] = { '\0' };
    if (query_args && strlen(query_args) > 0) {
        snprintf(internal_query, 1024, "partNumber=%d&uploadId=%s&%s",
                part_number, upload_id, query_args);
    } else {
        snprintf(internal_query, 1024, "partNumber=%d&uploadId=%s",
                part_number, upload_id);
    }
    buffer* resp = NULL;
    resp = buffer_init();
    make_header_buf(host, PUT_METHOD, bucket, object_key, part_data,
        part_data_len, internal_query, headers, access_key, secret_key, resp, err);
    return resp;
}

buffer* complete_multipart_upload(const char* host,
    const char* bucket, const char* object_key,
    const char* access_key, const char* secret_key,
    const char* upload_id, const char* parts_info, int parts_info_len,
    const char* query_args, const char* headers, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
    if (upload_id == NULL || strlen(upload_id) <= 0) {
        *err = -1;
        fprintf(stderr, "upload_id is NULL\n");
        return NULL;
    }
    if (parts_info == NULL || strlen(parts_info) <= 0 || parts_info_len <= 0) {
        *err = -1;
        fprintf(stderr, "parts_info is NULL\n");
        return NULL;
    }
    char internal_query[1024] = { '\0' };
    if (query_args && strlen(query_args) > 0) {
        snprintf(internal_query, 1024, "uploadId=%s&%s", upload_id, query_args);
    } else {
        snprintf(internal_query, 1024, "uploadId=%s", upload_id);
    }
    buffer* resp = NULL;
    resp = buffer_init();
    make_multiparts(host, POST_METHOD, bucket, object_key,
            parts_info, parts_info_len, internal_query, headers,
            MULTI_COMPLETE_OP, access_key, secret_key, resp, err);
    return resp;
}

buffer* abort_multipart_upload(const char* host,
    const char* bucket, const char* object_key, const char* access_key,
    const char* secret_key, const char* upload_id,
    const char* query_args, const char* headers, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
    if (upload_id == NULL || strlen(upload_id) <= 0) {
        *err = -1;
        fprintf(stderr, "upload_id is NULL\n");
        return NULL;
    }
    char internal_query[1024] = { '\0' };
    if (query_args && strlen(query_args) > 0) {
        snprintf(internal_query, 1024, "uploadId=%s&%s", upload_id, query_args);
    } else {
        snprintf(internal_query, 1024, "uploadId=%s", upload_id);
    }
    buffer* resp = NULL;
    resp = buffer_init();
    make_header(host, DELETE_METHOD, bucket, object_key, NULL, internal_query, 
        headers, access_key, secret_key, resp, err);
    return resp;
}

buffer* list_multipart_uploads(const char* host, const char* bucket,
    const char* access_key, const char* secret_key,
    const char* query_args, const char* headers, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
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
    const char* upload_id, const char* query_args,
    const char* headers, int* err) {
    if (check_params(access_key, secret_key, err) != 0) {
        return NULL;
    }
    if (upload_id == NULL || strlen(upload_id) <= 0) {
        *err = -1;
        fprintf(stderr, "upload_id is NULL\n");
        return NULL;
    }
    char internal_query[1024] = { '\0' };
    if (query_args && strlen(query_args) > 0) {
        snprintf(internal_query, 1024, "uploadId=%s&%s", upload_id, query_args);
    } else {
        snprintf(internal_query, 1024, "uploadId=%s", upload_id);
    }
    buffer* resp = NULL;
    resp = buffer_init();
    make_header(host, GET_METHOD, bucket, object_key, NULL, internal_query, headers, 
        access_key, secret_key, resp, err);
    return resp;
}


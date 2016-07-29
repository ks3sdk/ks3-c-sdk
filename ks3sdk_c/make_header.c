
#include <stdio.h>
#include <stdlib.h>
#include "curl/curl.h"
#include "make_header.h"
#include "util.h"
#include "sign.h"
#include "file_deal.h"

static void set_method(MethodType method_type, void* handler) {
	switch(method_type) {
	case PUT_METHOD:
		curl_easy_setopt(handler, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(handler, CURLOPT_PUT, 1L);
		break;
	case DELETE_METHOD:
		curl_easy_setopt(handler, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    case POST_METHOD:
        curl_easy_setopt(handler, CURLOPT_POST, 1L);
		break;
	}
}

static char* compute(const char* sign, const char* secret_key, char* b64) {
	unsigned char hmac[20];
	int b64Len;
	HMAC_SHA1(hmac, (const unsigned char *)secret_key,
		strlen(secret_key), (const unsigned char *)sign, strlen(sign));
	b64Len = base64Encode(hmac, 20, b64);
	b64[b64Len] = '\0';
	return b64;
}

static char* make_url(const char* host, const char* bucket_name,
	const char* object, const char* query_args, char* url) {
	if (host != NULL) {
	    strcat(url, host);
	}
	if (bucket_name != NULL) {
		strcat(url, "/");
		strcat(url, bucket_name);
		if (object != NULL) {
			strcat(url, "/");
			strcat(url, object);
		}
	}
	if (query_args) {
		//int size = strlen(query_args) * 3;
		char coded_query_args[1024] = { '\0' };
        //memset(coded_query_args, '\0', size);
        make_resource_quote(query_args, coded_query_args);
        if (strlen(coded_query_args) > 0) {
            strcat(url, "?");
            strcat(url, coded_query_args);
        }
	}
	strcat(url, "\0");
	return url;
}

static const char* HTTP = "HTTP";
static const char* CONTENT_LENGTH= "Content-Length";
static void parse_http_header(CURL* handler, buffer* resp) {
	char* token = NULL;
	char* out_ptr = NULL;
	char header_copy[201] = { '\0' };
	char content_len_str[100] = { '\0' };
	char* first_space = NULL;
	char* sec_space = NULL;

    if (handler == NULL || resp == NULL) {
        return;
    }
    // 1. get http status code
    curl_easy_getinfo(handler, CURLINFO_HTTP_CODE, &resp->status_code);
    if (resp->header == NULL) {
        return;
    }
    // 2. get http status msg
	strncpy(header_copy, resp->header, 200);
	// 2.1 get first line: start with 'HTTP'
#ifdef _WIN32
	token = strtok_s(header_copy, "\r\n", &out_ptr);    
#endif
#ifdef __linux__
	token = strtok_r(header_copy, "\r\n", &out_ptr);
#endif
    while (token != NULL) {
        if (strncmp(HTTP, token, strlen(HTTP)) == 0) {
            // 2.2 split "HTTP/xxxx status_code status_msg" by space
            first_space = strstr(token, " ");
            if (first_space == NULL) {
                return;
            }
            sec_space = strstr(first_space + 1, " ");
            // 2.3 copy status msg
            strcat(resp->status_msg, sec_space + 1);
        } else if (strncmp(CONTENT_LENGTH, token, strlen(CONTENT_LENGTH)) == 0) {
            first_space = strstr(token, ": ");
            if (first_space == NULL) {
                return;
            }   
            strcat(content_len_str, first_space + 2); 
            if (strlen(content_len_str) > 0) {
                resp->content_length = strtol(content_len_str, NULL, 10);
            }
            break;
        }
        first_space = NULL;
        sec_space = NULL;
#ifdef _WIN32
        token = strtok_s(NULL, "\r\n", &out_ptr);    
#endif
#ifdef __linux__
        token = strtok_r(NULL, "\r\n", &out_ptr);
#endif
    }
}

FILE* up_file_preprocess(MethodType method_type, const char* data,
        CURL* handler, buffer* resp) {
    FILE* file = NULL;
    size_t file_size;
    if (method_type == PUT_METHOD) {
        file = fopen(data, "rb");
        if (file == NULL) {
            return file;
        }
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        file_up(handler, file, file_size, resp);
    } else if (method_type == GET_METHOD) {
        file = fopen(data, "wb");
        if (file == NULL) {
            return file;
        }
        file_down(handler, file, resp);
    }
    return file;
}

void buf_preprocess(MethodType method_type, BufData* data,
        CURL* handler, buffer* resp) {
    if (method_type == PUT_METHOD) {
        buf_up(handler, data, data->len, resp);
    } else if (method_type == GET_METHOD) {
        buf_down(handler, resp);
    }
}

static int make_header_common(const char* host, MethodType method_type,
	const char* bucket, const char* object, const char* data, int buf_len,
	const char* query_args, const char* headers, OpType op_type,
	const char* access_key, const char* secret_key, buffer* resp) {
	
	FILE *file = NULL;
	char origin_sign[1024] = { '\0' };
	char url[1024] = { '\0' };
	char auth[1024] = { '\0' };
	char date_t[64] = { '\0' };
	char gtm[100] = { '\0' };
	char b64[((20 + 1) * 4) / 3+1] = { '\0' };
	CURLcode res;
	CURL* handler = NULL;
	struct curl_slist* http_header = NULL;

	// 1. make url
	make_url(host, bucket, object, query_args, url);
	// 2. get time
	get_time(date_t);
	strcat(gtm, "Date: ");
	strcat(gtm, date_t);
	// 3. make origin sign
	make_origin_sign(method_type, bucket, object,
		query_args, headers, date_t, origin_sign);
	// 4. compute base64_encoded sign
	strcat(auth, "Authorization: KSS ");
	strcat(auth, access_key);
	strcat(auth, ":");
	strcat(auth, compute(origin_sign, secret_key, b64));
	// 4. curl init
	curl_global_init(CURL_GLOBAL_ALL);
    handler = curl_easy_init();
	// 5. curl header append
	http_header = curl_slist_append(http_header, gtm);
    http_header = curl_slist_append(http_header, url);
    http_header = curl_slist_append(http_header, auth);
    http_header = curl_slist_append(http_header, "Transfer-Encoding:");
    http_header = curl_slist_append(http_header, "Expect:");
    http_header = curl_slist_append(http_header, "Accept:");
	if (headers != NULL) {
        http_header = curl_slist_append(http_header, headers);
    }
    
    if (method_type == POST_METHOD) {
        curl_easy_setopt(handler, CURLOPT_POSTFIELDS, query_args);
        //curl_easy_setopt(handler, CURLOPT_POSTFIELDSIZE, 0);
    }
	// 6. curl set op
	curl_easy_setopt(handler, CURLOPT_URL, url);
    set_method(method_type, handler);
    curl_easy_setopt(handler, CURLOPT_HTTPHEADER, http_header);

	// 7. curl perform
	BufData buf_data;
	do {
		if (op_type == FILE_OP) {
			file = up_file_preprocess(method_type, data, handler, resp);
			if (file == NULL) {
				break;
			}
		} else if (op_type == BUF_OP) {
			buf_data.data = data;
			buf_data.offset = 0;
			buf_data.len = buf_len;
			buf_preprocess(method_type, &buf_data, handler, resp);
		} else if (op_type == META_OP) {
			meta_deal(handler, NULL, resp, 0);
		} else if (op_type == MULTI_OP) {
            multipart_deal(handler, NULL, 0, resp);
        }
        else if (op_type == MULTI_COMPLETE_OP) {
            buf_data.data = data;
            buf_data.offset = 0;
            buf_data.len = buf_len;
            buf_up(handler, &buf_data, buf_data.len, resp);
            multipart_deal_down(handler, resp);
        }

		res = curl_easy_perform(handler);
        if (res == CURLE_OK) {
            parse_http_header(handler, resp);
		}
	} while (0);
	// 8. curl clean: free memory, else memory leak
	curl_easy_cleanup(handler);
	if(op_type == FILE_OP && file != NULL) {	
		fclose(file);
	}
	// free memory, else memory leak
	curl_slist_free_all(http_header);
	curl_global_cleanup();
	return res;
}

void make_header(const char* host, MethodType method_type, const char* bucket,
	const char* object,const char* filename, const char* query_args,
	const char* headers, const char* access_key, const char* secret_key,
	buffer* resp, int* err) {
	*err = make_header_common(host, method_type, bucket, object,
		filename, -1, query_args, headers, META_OP, access_key, secret_key, resp);
}

void make_header_file(const char* host, MethodType method_type, const char* bucket,
	const char* object, const char* filename, const char* query_args,
	const char* headers, const char* access_key, const char* secret_key,
	buffer* resp, int* err) {
	*err = make_header_common(host, method_type, bucket, object,
		filename, -1, query_args, headers, FILE_OP, access_key, secret_key, resp);
}

void make_header_buf(const char* host, MethodType method_type, const char* bucket,
    const char* object, const char* buf, int buf_len, const char* query_args,
    const char* headers, const char* access_key, const char* secret_key,
    buffer* resp, int* err) {
    *err = make_header_common(host, method_type, bucket, object,
        buf, buf_len, query_args, headers, BUF_OP, access_key, secret_key, resp);
}

void make_multiparts(const char* host, MethodType method_type, const char* bucket,
    const char* object, const char* buf_data, int buf_len, const char* query_args,
    const char* headers, OpType op_typeconst,  char* access_key, const char* secret_key,
    buffer* resp, int* err) {
    *err = make_header_common(host, method_type, bucket, object,
        buf_data, buf_len, query_args, headers, op_typeconst, access_key, secret_key, resp);
}

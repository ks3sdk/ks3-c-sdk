
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include "sign.h"
#include "make_header.h"

static const char* SUB_RESOURCES[15]={"acl", "lifecycle", "location", "logging", "notification", "partNumber", \
	"policy", "requestPayment", "torrent", "uploadId", "uploads", "versionId", \
	"versioning", "versions", "website"};

static const char* RESPONSE_OVERIDES[6]={"response-content-type", "response-content-language", \
	"response-expires", "response-cache-control", \
	"response-content-disposition", "response-content-encoding"};

static unsigned int num_of_sub   = sizeof(SUB_RESOURCES)/sizeof(char*);
static unsigned int num_of_rides = sizeof(RESPONSE_OVERIDES)/sizeof(char*);

static unsigned char hexchars[] = "0123456789ABCDEF";
static const char* specify_header = "x-kss";

static char * trim(char * src) {
	int i = 0;
	char* begin = src;
	while (src[i] != '\0') {
		if (src[i] != ' ') {
			break;
		} else {
			begin++;
		}
		i++;
	}
	for (i = strlen(src) - 1; i >= 0; i--) {
		if (src[i] != ' ') {
			break;
		} else {
			src[i] = '\0';
		}
	}
	return begin;
}

char *strlwr(char* str) {
	char *orign = str;
    if (str == NULL) {
        return NULL;
    }   
    for (; *str != '\0'; str++)
        *str = tolower(*str);
    return orign;
}

static int exist(const char** src, int num,
	const char* target) {
    int i;
    int exists = 0;
    for (i = 0; i< num; ++i) {
        if (0 == strcmp(src[i], target)) {
            exists = 1;
        }   
    }   
    return exists;
}

static void split_kv(const char* src, const char* delim, key_value* kv) {
    char* out_ptr = NULL;
	char* token = NULL;
	char* trim_token = NULL;
    char src_cpy[1024];
	int len;

    strcpy(src_cpy, src);
#ifdef _WIN32
    token = strtok_s(src_cpy, delim, &out_ptr);     
#endif
#ifdef __linux__
	token = strtok_r(src_cpy, delim, &out_ptr);     
#endif
    if (token == NULL) {
        return;
    }
    len = strlen(token);
    kv->key = (char *) malloc (len + 1);
    strncpy(kv->key, token, len);          
    kv->key[len] = '\0';
    if (len == strlen(src)) {
        return;
    }

	token = token + len + 1;
    trim_token = trim(token);
    if (trim_token != NULL) {
        len = strlen(trim_token);
        kv->value = (char *) malloc (len + 1);
        strncpy(kv->value, trim_token, len);
        kv->value[len] = '\0';
    }
}

static int key_compare(key_value* a, key_value* b) {
    return strcmp(a->key, b->key);
}

static int key_value_compare(key_value* a, key_value* b) {
    int ret = strcmp(a->key, b->key);
    if (ret != 0) {
        return ret;
    }
    return strcmp(a->value, b->value);
}

static void build_query_kv_map(const char* query_args, rb_node_t** root) {
	char* token = NULL;
	char* out_ptr = NULL;
	key_value* kv_pair = NULL;
	char src_cpy[1024] = { '\0' };
    strncpy(src_cpy, query_args, strlen(query_args));

	// 1. split by '&'
#ifdef _WIN32
    token = strtok_s(src_cpy, "&", &out_ptr);     
#endif
#ifdef __linux__
	token = strtok_r(src_cpy, "&", &out_ptr);     
#endif
    while (token != NULL) {
        // 2. split by '='
        kv_pair = (key_value *) malloc (sizeof(key_value));
        kv_pair->key = NULL;
        kv_pair->value = NULL;
        // key : no need tolower and trim
        // value : need to trim, no need tolower
        split_kv(token, "=", kv_pair);
        // 3. insert key_value into rb tree
        *root = rb_insert_compare(kv_pair, *root, &key_value_compare);
#ifdef _WIN32
        token = strtok_s(NULL, "&", &out_ptr);
#endif
#ifdef __linux__
		token = strtok_r(NULL, "&", &out_ptr);
#endif
    }
}

static void encode_value(const char* str_source,
	char* out_str) {
    const char *in_str = str_source;
    int in_str_len = strlen(in_str);

	int idx;
    register unsigned char c;
    unsigned char const *from, *end;

    from = (unsigned char *)in_str;
    end = (unsigned char *)in_str + in_str_len;

    idx = 0;
    while (from < end) {
        c = *from++;
        if (c == ' ') {
            out_str[idx++] = '+';
        } else if ((c < '0' && c != '-' && c != '.') ||
               (c < 'A' && c > '9') ||
                (c > 'Z' && c < 'a' && c != '_') ||
                (c > 'z')) {
            out_str[idx++] = '%';
            out_str[idx++] = hexchars[c >> 4];
            out_str[idx++] = hexchars[c & 15];
        } else {
            out_str[idx++] = c;
        }
    }
    out_str[idx] = '\0';
}

static void encode_kv_map(rb_node_t* root,
	int need_code, char* out_str) {
	key_value* cur_kv = NULL;
	char* coded_value = NULL;

	if (root == NULL) {
		return;
	}
	if (root->left != NULL) {
		encode_kv_map(root->left, need_code, out_str);
	}
	// encode
	cur_kv = root->kv;
	if (exist(SUB_RESOURCES, num_of_sub, cur_kv->key)
		|| exist(RESPONSE_OVERIDES, num_of_rides, cur_kv->key)) {
	    // join key=coded_value with &
        strcat(out_str, cur_kv->key);
        if (cur_kv->value != NULL) {
            strcat(out_str, "=");
            if (need_code) {
                int size = strlen(cur_kv->value) * 3;                                              
                coded_value = (char *) malloc (size);;
                memset(coded_value, '\0', size);
                encode_value(cur_kv->value, coded_value);
                strcat(out_str, coded_value);
                free(coded_value);
            } else {
                strcat(out_str, cur_kv->value);
            }
        }
        strcat(out_str, "&");
    }
	if (root->right != NULL) {
		encode_kv_map(root->right, need_code, out_str);
	}
}

char* make_resource_quote(const char* query_args, char* res) {
	int len;
	rb_node_t* root = NULL;
	if (query_args == NULL || strlen(query_args) <= 0) {
		return res;
	}
	// 1. build query_kv map
	build_query_kv_map(query_args, &root);
	// 2. iterate query_kv map and encode value
	encode_kv_map(root, 1, res);
	len = strlen(res);
	if (len > 0) {
		len = len - 1;
		memset(res + len, '\0', 1);
	}
	// 3. free kv map
	rb_erase_all(root);
	return res;
}

static void canon_resource(const char* bucket,
	const char* obj, const char* query_args, char* res) {
	int len;
	rb_node_t* root = NULL;
	char new_query_args[1024] = { '\0' };

    strcat(res, "/");
    if (bucket != NULL) {
        strcat(res, bucket);
        strcat(res, "/");
        if (obj != NULL) {
            strcat(res, obj);
        }
    }

    if (query_args != NULL
		&& strlen(query_args) > 0) {
		// 1. build query_kv map
		build_query_kv_map(query_args, &root);
		// 2. iterate query_kv map and encode value
		encode_kv_map(root, 0, new_query_args);
		len = strlen(new_query_args);
		if (len > 0) {
			len = len -1;
			memset(new_query_args + len, '\0', 1);
		}
		// 3. free kv map
		rb_erase_all(root);
		// 4. append to the tail of res
		strcat(res, "?");
        strcat(res, new_query_args);
    }
}

static void build_header_kv_map(const char* headers,
	rb_node_t** root) {
	char* token = NULL;
	char* out_ptr = NULL;
	key_value* kv_pair = NULL;
	char src_cpy[1024] = { '\0' };
    strncpy(src_cpy, headers, strlen(headers));

	// 1. split by '\n'
#ifdef _WIN32
    token = strtok_s(src_cpy, "\n", &out_ptr);     
#endif
#ifdef __linux__
	token = strtok_r(src_cpy, "\n", &out_ptr);     
#endif
    while (token != NULL) {
        // 2. split by ':'
        kv_pair = (key_value *) malloc (sizeof(key_value));        
        kv_pair->key = NULL;
        kv_pair->value = NULL;
		// key : need tolower, no need to trim
        // value : need to trim, no need tolower
        split_kv(token, ":", kv_pair);
		strlwr(kv_pair->key);
        // 3. insert key_value into rb tree
        *root = rb_insert_compare(kv_pair, *root, &key_compare);
#ifdef _WIN32
        token = strtok_s(NULL, "\n", &out_ptr);
#endif
#ifdef __linux__
		token = strtok_r(NULL, "\n", &out_ptr);
#endif
    }
}

static void canon_headers(const rb_node_t* root,
        char* out_headers) {
	key_value* cur_kv = NULL;
	if (root == NULL) {
		return;
	}
	if (root->left != NULL) {
		canon_headers(root->left, out_headers);
	}
	cur_kv = root->kv;
	// compare key with specify_header and join kv pairs with '\n'
	if (strncmp(cur_kv->key, specify_header, strlen(specify_header)) == 0) {
		strcat(out_headers, cur_kv->key);
		if (cur_kv->value != NULL) {
            strcat(out_headers, ":");
            strcat(out_headers, cur_kv->value);
            strcat(out_headers, "\n");
		}
	}
	if (root->right != NULL) {
		canon_headers(root->right, out_headers);
	}
}

extern char* make_origin_sign(int method_type,
	const char* bucket, const char* obj,
	const char* query_args, const char* headers,
	const char* time, char* origin_sign) {

	static const char* method[MethodType_END] = {"GET", "PUT", "DELETE", "POST"};
	struct rb_node_t* root = NULL;
	key_value kv;
	rb_node_t* md5_node = NULL;
	rb_node_t* type_node = NULL;
	char canned_headers[1024] = { '\0' };
    
	// 1. append: ${method}\n
    strcpy(origin_sign, method[method_type]);
    strcat(origin_sign, "\n");
    // 2. build header map
	if (headers != NULL) {
		build_header_kv_map(headers, &root);
	}
    // 3. get and append: ${content-md5}\n
	kv.key = "content-md5";
    md5_node = rb_search_compare(&kv, root, &key_compare);
	if (md5_node != NULL
		&& md5_node->kv != NULL && md5_node->kv->value != NULL) {
        strcat(origin_sign, md5_node->kv->value);
    }
    strcat(origin_sign, "\n");
    // 4. get and append: ${content-type}\n
	kv.key = "content-type";
    type_node = rb_search_compare(&kv, root, &key_compare);
    if (type_node != NULL
		&& type_node->kv != NULL && type_node->kv->value != NULL) {
        strcat(origin_sign, type_node->kv->value);
    }
    strcat(origin_sign, "\n");
    // 5. append: ${date_time}\n
    strcat(origin_sign, time);
    strcat(origin_sign, "\n");
    // 6. canon headers, keys start with 'x-kss'
	if (headers != NULL) {
		canon_headers(root, canned_headers);
		strcat(origin_sign, canned_headers);
	}
	// 7. canon query_args
	canon_resource(bucket, obj, query_args, origin_sign);
    // 8. free header map
	rb_erase_all(root);
	return origin_sign;
}

extern char* get_time(char* out_time) {	
	char date[64] = { '\0' };
	time_t now = time(NULL);
	strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));
	strncpy(out_time, date, sizeof(date));
	return out_time;
}

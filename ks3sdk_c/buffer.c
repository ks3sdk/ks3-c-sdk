
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"

#define BUFFER_PIECE_SIZE       64
#define BUFFER_MAX_REUSE_SIZE   (4 * 1024)
#define CONTENT_PIECE_SIZE      512000

buffer* buffer_init(void) {
    buffer* b;

    b = (buffer *)malloc(sizeof(buffer));
    assert(b);

    b->status_code = -1;
    memset(b->status_msg, '\0', sizeof(b->status_msg));
    b->content_length = -1;

    b->header = NULL;
    b->header_used = 0;
    b->header_size = 0;

    b->body = NULL;
    b->body_used = 0;
    b->body_size = 0;

    b->content = NULL;
    b->content_used = 0;
    b->content_size = 0;

    return b;
}

void buffer_free(buffer* b) {
    if (!b) return;

    if (b->header != NULL) {
        free(b->header);
        b->header = NULL;
    }
    if (b->body != NULL) {
        free(b->body);
        b->body = NULL;
    }
    if (b->content != NULL) {
        free(b->content);
        b->content = NULL;
    }
    free(b);
    b = NULL;
}

int buffer_header_prepare_append(buffer* b, size_t size) {
    if (!b) return -1;
    if (0 == size) return 0;
    size += 1;

    if (0 == b->header_size) {
        b->header_size = size;
        b->header_size += BUFFER_PIECE_SIZE - (b->header_size % BUFFER_PIECE_SIZE);
        b->header = (char *)malloc(b->header_size);
        assert(b->header);
    } else if (b->header_used + size > b->header_size) {
        b->header_size += size;
        b->header_size += BUFFER_PIECE_SIZE - (b->header_size % BUFFER_PIECE_SIZE);
        b->header = (char *)realloc(b->header, b->header_size);
        assert(b->header);
    }

    return 0;
}

int buffer_header_append_string_len(buffer* b, const char* s, size_t len) {
    if (b == NULL) return -1;
	if (s == NULL) return -2;
    if (0 == len) return 0;
	
    buffer_header_prepare_append(b, len);

    memcpy(b->header + b->header_used, s, len);
    b->header_used += len;

    b->header[b->header_used] = '\0';

    return len;
}

//int buffer_body_prepare_append(buffer* b, size_t size) {
//    if (!b) return -1;
//    if (0 == size) return 0;
//    size += 1;
//
//    if (0 == b->body_size) {
//        b->body_size = size;
//        b->body_size += BUFFER_PIECE_SIZE - (b->body_size % BUFFER_PIECE_SIZE);
//        b->body = (char *)malloc(b->body_size);
//        assert(b->body);
//    } else if (b->body_used + size > b->body_size) {
//        b->body_size += size;
//        b->body_size += BUFFER_PIECE_SIZE - (b->body_size % BUFFER_PIECE_SIZE);
//        b->body = (char *)realloc(b->body, b->body_size);
//        assert(b->body);
//    }
//
//    return 0;
//}
int buffer_body_prepare_append(buffer* b, size_t size) {
    char *new_ptr = NULL;
    int  new_size = 0;
    
    if (!b) return -1;
    if (0 == size) return 0;
    size += 1;
    
    if (b->body_used + size > b->body_size) {
        new_size = b->body_used + size;
        new_size += BUFFER_MAX_REUSE_SIZE - (b->body_size % BUFFER_MAX_REUSE_SIZE);

        new_ptr = (char *)malloc(new_size);
        assert(new_ptr);
        if (b->body_used > 0 && b->body)
            memcpy(new_ptr, b->body, b->body_used);
        if (b->body) 
            free(b->body);
        b->body = new_ptr;
        b->body_size = new_size;
    }
    return 0;
}

int buffer_body_append_string_len(buffer* b, const char* s, size_t len) {
    if (!b) return -1;
	if (!s) return -2;
    if (0 == len) return 0;

    buffer_body_prepare_append(b, len);

    memcpy(b->body + b->body_used, s, len);
    b->body_used += len;

    b->body[b->body_used] = '\0';

    return len;
}

int buffer_content_prepare_append(buffer* b, size_t size) {
    if (!b) return -1;
    if (0 == size) return 0;

    if (0 == b->content_size) {
        b->content_size = size;
        b->content_size += CONTENT_PIECE_SIZE - (b->content_size % CONTENT_PIECE_SIZE);
        b->content = (char *)malloc(b->content_size);
        assert(b->content);
    } else if (b->content_used + size > b->content_size) {
        b->content_size += size;
        b->content_size += CONTENT_PIECE_SIZE - (b->content_size % CONTENT_PIECE_SIZE);
        b->content = (char *)realloc(b->content, b->content_size);
        assert(b->content);
    }

    return 0;
}

int buffer_content_append_len(buffer* b, const char* s, size_t len) {
    if (!b) return -1;
	if (!s) return -2;
    if (0 == len) return 0;

    buffer_content_prepare_append(b, len);

    memcpy(b->content + b->content_used, s, len);
    b->content_used += len;

    return len;
}

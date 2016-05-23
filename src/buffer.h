
#ifndef _KS3_SDK_C_BUFFER_H_
#define _KS3_SDK_C_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

typedef struct buffer_s buffer;

struct buffer_s {
    int64_t status_code;
    char status_msg[50];

    char* header;
    size_t header_used;
    size_t header_size;

    char* body;
    size_t body_used;
    size_t body_size;                                                                                                 
};

extern buffer* buffer_init(void);
extern void buffer_free(buffer* b);

extern int buffer_header_prepare_append(buffer* b, size_t size);
extern int buffer_header_append_string_len(buffer* b, const char* s, size_t len);

extern int buffer_body_prepare_append(buffer* b, size_t size);
extern int buffer_body_append_string_len(buffer* b, const char* s, size_t len);

#ifdef __cplusplus
}
#endif
#endif  // _KS3_SDK_C_BUFFER_H_

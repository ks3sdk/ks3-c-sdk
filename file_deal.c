
#include "file_deal.h"
#include "buffer.h"

static size_t read_file(void *ptr, size_t size, size_t nmemb, void *file) {
    size_t sizes = fread(ptr, size, nmemb, (FILE *)file); 
    return sizes; 
}

static size_t read_buf(void *ptr, size_t size, size_t nmemb, void *buf) {
	BufData* buf_data = (BufData *) buf;
	if (buf_data->offset >= buf_data->len) {
		return 0;
	}
	int32_t to_read_len = nmemb * size;
	int32_t remain_len = buf_data->len - buf_data->offset;
	if (to_read_len > remain_len) {
		to_read_len = remain_len;
	}
	memcpy(ptr, buf_data->data + buf_data->offset, to_read_len);
	buf_data->offset += to_read_len;
	return to_read_len;
}

static size_t write_file(void *ptr, size_t size, size_t nmemb, void* out) {
    return fwrite(ptr, size, nmemb, (FILE*)out);
}

static size_t read_s3_string_data(void *ptr, size_t size,
        size_t nmemb, void *stream) {
    size_t num_bytes = size * nmemb;
    memcpy(ptr, stream, num_bytes);
    return num_bytes;
}

static size_t read_http_header_resp(void *ptr, size_t size,
        size_t nmemb, void *stream) {
	int ret;
    size_t num_bytes = size * nmemb;
    buffer* header = (buffer*)stream;
    ret = buffer_header_append_string_len(header, (const char*)ptr, num_bytes);
    return num_bytes;
}

static size_t read_s3_response(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t num_bytes = size * nmemb;
    buffer* resp = (buffer*)stream;
    buffer_body_append_string_len(resp, (const char*)ptr, num_bytes);
    return num_bytes;
}

static void meta_deal_up(void *handler, void* ptr, size_t size) {
    // set conn time and transfer time
    curl_easy_setopt(handler, CURLOPT_CONNECTTIMEOUT, 10 * 1000);
    curl_easy_setopt(handler, CURLOPT_TIMEOUT_MS, 20 * 60 * 1000);
    // set read function and file
    curl_easy_setopt(handler, CURLOPT_READFUNCTION, &read_s3_string_data);
    curl_easy_setopt(handler, CURLOPT_READDATA, ptr);
    curl_easy_setopt(handler, CURLOPT_INFILESIZE, size);
}

static void meta_deal_down(void *handler, buffer* resp) {
    // read header
    curl_easy_setopt(handler, CURLOPT_HEADERFUNCTION, read_http_header_resp);
    curl_easy_setopt(handler, CURLOPT_HEADERDATA, resp);
    // read body
    curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, read_s3_response);
    curl_easy_setopt(handler, CURLOPT_WRITEDATA, resp);
    /*
     * if set CURLOPT_VERSION as non-zero value,
     * show req header and resp header info in stdout
     * */
    //curl_easy_setopt(handler, CURLOPT_VERBOSE, 1);
}

static void file_deal_up(void *handler, FILE* file,
        curl_off_t size, buffer* resp) {
    // set conn time and transfer time
    curl_easy_setopt(handler, CURLOPT_CONNECTTIMEOUT, 10 * 1000);
    curl_easy_setopt(handler, CURLOPT_TIMEOUT_MS, 20 * 60 * 1000);
    // set read function and file
    curl_easy_setopt(handler, CURLOPT_READFUNCTION, read_file);
    curl_easy_setopt(handler, CURLOPT_READDATA, file);

    curl_easy_setopt(handler, CURLOPT_INFILE, file);
    curl_easy_setopt(handler, CURLOPT_INFILESIZE_LARGE, size);
    // read header
    curl_easy_setopt(handler, CURLOPT_HEADERFUNCTION, read_http_header_resp);
    curl_easy_setopt(handler, CURLOPT_HEADERDATA, resp);
    // read body
    curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, read_s3_response);
    curl_easy_setopt(handler, CURLOPT_WRITEDATA, resp);
    /*
     * if set CURLOPT_VERSION as non-zero value,
     * show req header and resp header info in stdout
     * */
    //curl_easy_setopt(handler, CURLOPT_VERBOSE, 1);
}

static void file_deal_down(void *handler, FILE* file, buffer* resp) {
    // set conn time and transfer time
    curl_easy_setopt(handler, CURLOPT_CONNECTTIMEOUT, 10 * 1000);
    curl_easy_setopt(handler, CURLOPT_TIMEOUT_MS, 20 * 60 * 1000);
    // read header
    curl_easy_setopt(handler, CURLOPT_HEADERFUNCTION, read_http_header_resp);
    curl_easy_setopt(handler, CURLOPT_HEADERDATA, resp);
    // read body
    curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, write_file);
    curl_easy_setopt(handler, CURLOPT_WRITEDATA, file);
    /*
     * if set CURLOPT_VERSION as non-zero value,
     * show req header and resp header info in stdout
     * */
    //curl_easy_setopt(handler, CURLOPT_VERBOSE, 1);
}

void meta_deal(void *handler, void* ptr, buffer* resp, size_t size) {
    meta_deal_up(handler, ptr, size);
    meta_deal_down(handler, resp);
}

void file_up(void *handler, FILE* file, curl_off_t size, buffer* resp) {
    file_deal_up(handler, file, size, resp);
}

void file_down(void *handler, FILE* file, buffer* resp) {
    file_deal_down(handler, file, resp);
}

void buf_up(void *handler, BufData* buf_data, curl_off_t size, buffer* resp) {
    // set conn time and transfer time
    curl_easy_setopt(handler, CURLOPT_CONNECTTIMEOUT, 10 * 1000);
    curl_easy_setopt(handler, CURLOPT_TIMEOUT_MS, 20 * 60 * 1000);
    // set read function and file
    curl_easy_setopt(handler, CURLOPT_READFUNCTION, read_buf);
    curl_easy_setopt(handler, CURLOPT_READDATA, buf_data);

    curl_easy_setopt(handler, CURLOPT_INFILE, buf_data);
    curl_easy_setopt(handler, CURLOPT_INFILESIZE_LARGE, size);
    // read header
    curl_easy_setopt(handler, CURLOPT_HEADERFUNCTION, read_http_header_resp);
    curl_easy_setopt(handler, CURLOPT_HEADERDATA, resp);
    // read body
    curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, read_s3_response);
    curl_easy_setopt(handler, CURLOPT_WRITEDATA, resp);
}

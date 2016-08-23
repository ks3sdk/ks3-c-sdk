/*************************************************************************
	> File Name: load_key.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Wed 18 May 2016 04:15:28 PM CST
 ************************************************************************/


char ak[100];
char sk[100];
char host[512];

extern int load_ak_sk();
extern int CreateBucket(const char* host, const char* bucket);
extern int DeleteBucket(const char* host, const char* bucket);
extern void GetUploadId(const char* in_str, char* out_str);
extern const char* CreateBigFile(const char* file, int64_t file_size);
extern void RemoveFile(const char *file);


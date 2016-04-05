/*
 * @landyliu
*/

#ifndef PANDORA_UTIL_FILE_UTIL_H_ 
#define PANDORA_UTIL_FILE_UTIL_H_ 1 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

namespace pandora 
{

#ifndef S_IRWXUGO
#define S_IRWXUGO (S_IRWXU | S_IRWXG | S_IRWXO)
#endif

// @brief 对文件目录的基本操作 
class FileUtil
{
public:
    /** create multi-level dirs */
    static bool Mkdirs(char *dir_path);
    static bool IsDirectory(const char *dir_path);
    static bool IsSymLink(const char *dir_path);

private:
    FileUtil();
    ~FileUtil();
};

}   // end of namespace

#endif


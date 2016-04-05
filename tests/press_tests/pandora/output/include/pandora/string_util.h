/*
 * @landyliu
 */

#ifndef PANDORA_UTIL_STRING_UTIL_H_
#define PANDORA_UTIL_STRING_UTIL_H_ 1 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <string>
#include <time.h>
#include <vector>
#include "ascii.h"


namespace pandora
{

class StringUtil
{
public:
    //@para d, the default value returned if convert failed
    static int      StrToInt(const char *str, int d);
    static bool     IsInt(const char *str);
    //@para str, [IN|OUT] revised in-place
    static char *   StrToLower(char *str);
    //@para str, [IN|OUT] revised in-place
    static char *   StrToUpper(char *str);
    /*
     * @brief trim the spaces at either/both ends of the string
     * @para str,   [OUT] source string
     * @para what,  need to be trimmed chars 
     * @para mode, 
     *              1 - trim the beginning spaces
     *              2 - trim the ending spaces
     *              3 - trim both ends  
     */
    static char *   Trim(char *str, const char *what = " ", int mode = 3);
    /* 
     * @brief split string by delim and put the result into list
     * @para str,   [OUT]
     * @para list,  [OUT]
     */
    static void     Split(char *str, const char *delim, std::vector<char*> &list);
    static char *   UrlDecode(const char *src, char *dest);
    //@brief convert bytes to readable KB MB, such as 10K, 12M etc.
    static std::string FormatByteSize(double bytes);

//hash functions:
    static int HashCode(const char *str);
    // get a prime hash value 
    static int GetPrimeHash(const char *str);
    // http://murmurhash.googlepages.com/
    static unsigned int MurMurHash(const void *key, int len);

private:
    StringUtil();
    ~StringUtil();
};

int32_t StringTrim(std::string &str_from);

}   //end of namespace
#endif

/*
 * @landyliu
*/

#ifndef PANDORA_UTIL_TIME_UTIL_H_
#define PANDORA_UTIL_TIME_UTIL_H_ 1 

#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

namespace pandora 
{

class TimeUtil 
{
public:
    // Get current time, the return value unit is microsecond
    static int64_t GetTime();
    // Get Monotonic Time, the return value unit is microsecond
    static int64_t GetMonotonicTime(); 
    /* @brief format time to '20120201112723'
     * @para dest, [OUT]
     */
    static char * TimeToStr(time_t t, char *dest);
    /* @brief format time like 2012-02-01 11:27:23 
     * @para dest, [OUT] 
     */
    static char * TimeToLocalTimeStr(time_t t, char *dest);
    // transfer time format '20120201112723' to int seconds
    static int StrToTime(char *str);

private:
    TimeUtil();
    ~TimeUtil();
};

}
#endif

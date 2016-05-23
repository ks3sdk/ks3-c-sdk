/*
 * @landyliu
*/

#ifndef PANDORA_SYSTEM_TIME_H
#define PANDORA_SYSTEM_TIME_H

#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <sstream>

namespace pandora
{

class Time
{
public:
    Time();
    Time(int64_t);

    static Time GetRealTime();
    static Time GetMonotonicTime();
    /*
     * Construct a time obj
     * @param sec:[INPUT], time unit is sec 
     */
    static Time Seconds(int64_t sec);
    /*
     * Construct a time obj
     * @param milli:[INPUT], time unit is millisecond
     */
    static Time MilliSeconds(int64_t milli);
    /*
     * Construct a time obj
     * @param micro:[INPUT], time unit is microsecond
     */
    static Time MicroSeconds(int64_t micro);

    // Transform microsecond to struct timeval
    operator timeval() const;
    // Transform time to seconds
    int64_t ToSeconds() const;
    // Transform time to milliseconds
    int64_t ToMilliSeconds() const;
    // Transform time to microseconds
    int64_t ToMicroSeconds() const;
    // Transform time to string, eg: 2012-02-01 16:05:05.433
    std::string ToDateTime() const;

private:
    int64_t usec_;
};
}
#endif

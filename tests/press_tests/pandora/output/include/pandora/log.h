/*
 * @landyliu
 */

#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>
#include <pthread.h>
#include <stdint.h>
#include <string>

#define LL_FATAL                 0
#define LL_ERROR                 1
#define LL_WARN                  2
#define LL_NOTICE                3
#define LL_INFO                  4
#define LL_DEBUG                 5

#define MSG_FATAL                LL_FATAL, __FILE__, __LINE__, __FUNCTION__
#define MSG_ERROR                LL_ERROR, __FILE__, __LINE__, __FUNCTION__
#define MSG_WARN                 LL_WARN, __FILE__, __LINE__, __FUNCTION__
#define MSG_NOTICE               LL_NOTICE, __FILE__, __LINE__, __FUNCTION__
#define MSG_INFO                 LL_INFO, __FILE__, __LINE__, __FUNCTION__
#define MSG_DEBUG                LL_DEBUG, __FILE__, __LINE__, __FUNCTION__

#define LOGGER                   ::pandora::Log::logger_
#define LOG(level, fmt, args...) if (level <= LOGGER.log_level()) LOGGER.Write(level, fmt, ##args)
#define LOGV(level, fmt, args...) if (level <= LOGGER.log_level()) LOGGER.Write(level, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)

#ifdef PANDORA_DEBUG
#define DEBUG(fmt, args...)      LOG(LL_DEBUG, fmt, ##args)
#define DEBUGV(fmt, args...)     LOG(MSG_DEBUG, fmt, ##args)
#else
#define DEBUG(fmt, args...)
#define DEBUGV(fmt, args...)
#endif

namespace pandora {
const int kLogMaxRowLength = 8192;

class Log
{
public:
    static Log logger_;

public:
    Log(const char *path = "",
        const char *prefix = "",
        int32_t level = LL_DEBUG,
        int64_t max_file_size = 0x1400000,   //20M
        int32_t max_file_count = 10);
    ~Log();

    void set_log_level(int32_t level);
    int32_t log_level() {return log_level_;}
    void set_max_file_size(int64_t max_file_size);
    void set_max_file_count(int32_t file_count);
    void set_need_process_id(bool value) { need_process_id_ = value; }
    void set_need_thread_id(bool value) { need_thread_id_ = value; }

    bool SetLogPrefix(const char *path, const char *prefix);
    void Write(int32_t level, const char *fmt, ...);
    void Write(int32_t level, const char *filename, 
               int32_t line, const char *function, 
               const char *fmt, ...);
    void Write(int32_t level, const char *filename, 
               int32_t line, const char *function, 
               const char *fmt, va_list args);

private:
    bool OpenLog();
    void CloseLog();
    bool GetLogName(int32_t index, std::string& filename);
    void SetDefaultParam();
    bool WriteData(const char *data, int32_t size);
    void RotateLog();

    std::string path_;
    int32_t fd_;
    pthread_mutex_t lock_;
    int64_t max_file_size_;
    int32_t max_file_count_;
    int32_t exist_file_count_;
    int32_t log_level_;
    bool need_process_id_;
    bool need_thread_id_;
};

} // end namespace pandora

#endif


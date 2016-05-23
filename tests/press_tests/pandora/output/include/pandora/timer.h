/*
 * @landyliu
*/

#ifndef PANDORA_TIMER_H
#define PANDORA_TIMER_H

#include <set>
#include <map>
#include "timer_task.h"
#include "shared.h"
#include "thread.h"
#include "monitor.h"
#include "time_wrap.h"
#include "log.h"

namespace pandora 
{
/*
 * Timer class
 */
class Timer :public virtual Shared ,private virtual pandora::Thread
{
public:
    Timer();
    // schedule a task, only run once
    int Schedule(const TimerTaskPtr& task, const Time& delay);
    // schedule a task and repeat running
    int ScheduleRepeated(const TimerTaskPtr& task, const Time& delay);
    // cancel a task
    bool Cancel(const TimerTaskPtr&);
    // stop timer
    void Destroy();

private:
    struct Token
    {
        Time scheduled_time;
        Time delay;
        TimerTaskPtr task;
        inline Token(const Time&, const Time&, const TimerTaskPtr&);
        inline bool operator<(const Token& r) const;
    };

    virtual void Run();

    Monitor<Mutex> monitor_;
    bool destroyed_;
    std::set<Token> tokens_;
    
    class TimerTaskCompare : public std::binary_function<TimerTaskPtr, TimerTaskPtr, bool>
    {
    public:
        bool operator()(const TimerTaskPtr& lhs, const TimerTaskPtr& rhs) const
        {
            return lhs.Get() < rhs.Get();
        }
    };
    std::map<TimerTaskPtr, Time, TimerTaskCompare> tasks_;
    Time wakeup_time_;
};
typedef Handle<Timer> TimerPtr;

inline 
Timer::Token::Token(const Time& st, const Time& d, const TimerTaskPtr& t) :
    scheduled_time(st), delay(d), task(t)
{
}

inline bool
Timer::Token::operator<(const Timer::Token& r) const
{
    if(scheduled_time.ToMicroSeconds() < r.scheduled_time.ToMicroSeconds())
    {
        return true;
    }
    else if(scheduled_time.ToMicroSeconds() > r.scheduled_time.ToMicroSeconds())
    {
        return false;
    }
    return task.Get() < r.task.Get();
}

}

#endif


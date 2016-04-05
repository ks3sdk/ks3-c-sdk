/*
 * @landyliu
*/

#ifndef PANDORA_COND_H
#define PANDORA_COND_H

#include <assert.h>
#include <errno.h>
#include "time_wrap.h"
#include "log.h"

namespace pandora
{
template<class T> class Monitor;
class Mutex;

// linux thread conditon variable
class Cond
{
public:
    Cond();
    ~Cond();

    void Signal();
    void Broadcast();

    //thread block wait
    template <typename Lock> inline bool 
    Wait(const Lock& lock) const
    {
        if(!lock.acquired())
        {
            LOG(LL_ERROR,"%s","ThreadLockedException");
            return false;
        }
        return WaitImpl(lock.mutex_);
    }

    // thread block wait until the time is over
    template <typename Lock> inline bool
    TimedWait(const Lock& lock, const Time& timeout) const
    {
        if(!lock.acquired())
        {
            LOG(LL_ERROR,"%s","ThreadLockedException");
            return false;
        }
        return TimedWaitImpl(lock.mutex_, timeout);
    }

private:

    friend class Monitor<Mutex>;

    template <typename M> bool WaitImpl(const M&) const;
    template <typename M> bool TimedWaitImpl(const M&, const Time&) const;

    mutable pthread_cond_t cond_;
};

template <typename M> inline bool 
Cond::WaitImpl(const M& mutex) const
{
    typedef typename M::LockState LockState;
    
    LockState state;
    mutex.Unlock(state);
    const int rc = pthread_cond_wait(&cond_, state.mutex);
    mutex.Lock(state);
    
    if( 0 != rc )
    {
        LOG(LL_ERROR,"%s","ThreadSyscallException");
        return false;
    } 
    return true;
}

template <typename M> inline bool
Cond::TimedWaitImpl(const M& mutex, const Time& timeout) const
{
    if(timeout.ToMicroSeconds() < Time::MicroSeconds(0).ToMicroSeconds())
    {
        LOG(LL_ERROR,"%s","InvalidTimeoutException");
        return false;
    }

    typedef typename M::LockState LockState;
    
    LockState state;
    mutex.Unlock(state);
   
    timeval tv = Time::MicroSeconds(Time::GetRealTime().ToMicroSeconds() + timeout.ToMicroSeconds());
    timespec ts;
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
    const int rc = pthread_cond_timedwait(&cond_, state.mutex, &ts);
    mutex.Lock(state);
    if(rc != 0)
    {
        if ( rc != ETIMEDOUT )
        {
            LOG(LL_ERROR,"%s","ThreadSyscallException");
            return false;
        }
    }
    return true;
}

}// end namespace 
#endif

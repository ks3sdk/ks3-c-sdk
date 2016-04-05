/*
 * @landyliu
*/

#ifndef PANDORA_MONITOR_H
#define PANDORA_MONITOR_H

#include <iostream>
#include "lock.h"
#include "cond.h"

namespace pandora
{
/*
 * Monitor is a template class. It needs Mutex as its template param.
 * Make use of both mutex and cond.
 */
template <class T>
class Monitor
{
public:

    typedef LockT<Monitor<T> > MO_Lock;
    typedef TryLockT<Monitor<T> > MO_TryLock;

    Monitor();
    ~Monitor();

    void Lock() const;
    void Unlock() const;
    bool TryLock() const;
    bool Wait() const;
    bool TimedWait(const Time&) const;
    void Notify();
    void NotifyAll();

private:

    Monitor(const Monitor&);
    Monitor& operator=(const Monitor&);

    void NotifyImpl(int) const;

    mutable Cond cond_;
    T mutex_;
    mutable int nnotify_;
};

template <class T> 
Monitor<T>::Monitor() :
    nnotify_(0)
{
}

template <class T> 
Monitor<T>::~Monitor()
{
}

template <class T> inline void
Monitor<T>::Lock() const
{
    mutex_.Lock();
    if(mutex_.WillUnlock())
    {
        nnotify_ = 0;
    }
}

template <class T> inline void
Monitor<T>::Unlock() const
{
    if(mutex_.WillUnlock())
    {
        NotifyImpl(nnotify_);
    }
    mutex_.Unlock();
}

template <class T> inline bool
Monitor<T>::TryLock() const
{
    bool result = mutex_.TryLock();
    if(result && mutex_.WillUnlock())
    {
        nnotify_ = 0;
    }
    return result;
}

template <class T> inline bool 
Monitor<T>::Wait() const
{
    NotifyImpl(nnotify_);
    const bool bRet = cond_.WaitImpl(mutex_);
    nnotify_ = 0;
    return bRet;
}

template <class T> inline bool
Monitor<T>::TimedWait(const Time& timeout) const
{
    NotifyImpl(nnotify_);
    const bool rc = cond_.TimedWaitImpl(mutex_, timeout);
    nnotify_ = 0;
    return rc;
}

template <class T> inline void
Monitor<T>::Notify()
{
    if(nnotify_ != -1)
    {
        ++nnotify_;
    }
}

template <class T> inline void
Monitor<T>::NotifyAll()
{
    nnotify_ = -1;
}


template <class T> inline void
Monitor<T>::NotifyImpl(int nnotify) const
{
    if(nnotify != 0)
    {
        if(nnotify == -1)
        {
            cond_.Broadcast();
            return;
        }
        else
        {
            while(nnotify > 0)
            {
                cond_.Signal();
                --nnotify;
            }
        }
    }
}
}//end namespace
#endif

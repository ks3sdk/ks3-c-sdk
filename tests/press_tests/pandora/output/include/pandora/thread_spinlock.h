/*
 * @landyliu
*/

#ifndef PANDORA_SYSTEM_THREAD_THREAD_SPINLOCK_H
#define PANDORA_SYSTEM_THREAD_THREAD_SPINLOCK_H

#include <stdio.h>
#include <pthread.h>

namespace pandora
{

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName)\
                    TypeName(const TypeName&);\
                void operator=(const TypeName&)
#endif

class ThreadSpinLock
{
public:
    ThreadSpinLock();
    ~ThreadSpinLock();

    void Lock();   
    bool TryLock();
    void Unlock();

private:
    int lock_init_;
    pthread_spinlock_t lock_;

    DISALLOW_COPY_AND_ASSIGN(ThreadSpinLock);
};

/////////////////////////////////////////////
class ScopeSpinLock
{
public:
    ScopeSpinLock();
    ~ScopeSpinLock();

private:
    ThreadSpinLock lock_;

    DISALLOW_COPY_AND_ASSIGN(ScopeSpinLock);
};

} //end of namespace pandora
#endif 

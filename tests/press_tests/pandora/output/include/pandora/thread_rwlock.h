/*
 * @landyliu
*/

#ifndef PANDORA_SYSTEM_THREAD_THREAD_RWLOCK_H
#define PANDORA_SYSTEM_THREAD_THREAD_RWLOCK_H

#include <pthread.h>
#include <assert.h>

namespace pandora 
{
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName)\
                    TypeName(const TypeName&);\
                void operator=(const TypeName&)
#endif


////////////////////////
class CThreadRWLock 
{
public:
    CThreadRWLock();
    ~CThreadRWLock();

    void RdLock();
    void WrLock();
    int TryRdLock();
    int TryWrLock();
    void Unlock();
private:
    pthread_rwlock_t rwlock_;
    DISALLOW_COPY_AND_ASSIGN(CThreadRWLock);
};

////////////////////////////
class CThreadWrGuard
{
public:
    explicit CThreadWrGuard(CThreadRWLock * rwlock);
    ~CThreadWrGuard();

private:
    CThreadRWLock * rwlock_;
    DISALLOW_COPY_AND_ASSIGN(CThreadWrGuard);
};

////////////////////////////
class CThreadRdGuard
{
public:
    explicit CThreadRdGuard(CThreadRWLock * rwlock);
    ~CThreadRdGuard();

private:
    CThreadRWLock * rwlock_;
    DISALLOW_COPY_AND_ASSIGN(CThreadRdGuard);
};
}   // namespace pandora
#endif  // PANDORA_SYSTEM_THREAD_THREAD_RWLOCK_H 

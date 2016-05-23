/*
 * @landyliu
 */

#ifndef PANDORA_SYSTEM_LOCK_RWLOCK_H_ 
#define PANDORA_SYSTEM_LOCK_RWLOCK_H_ 1 

#include <pthread.h>
#include "lock_guard.h"

namespace pandora 
{

enum ELockMode
{
    NO_PRIORITY,
    WRITE_PRIORITY,
    READ_PRIORITY
};

////////////////////////////////////////////////////
// @brief wrap linux thread read lock
class CRLock
{
public:
    CRLock(pthread_rwlock_t * lock) : rlock_(lock) {}
    ~CRLock() {}
    
    int Lock() const;
    int Trylock() const;
    int Unlock() const;
    
private:
    mutable pthread_rwlock_t* rlock_;
};

////////////////////////////////////////////////////
// @brief linux thread write lock wrap 
class CWLock
{
public:
    CWLock(pthread_rwlock_t* lock) : wlock_(lock) {}
    ~CWLock() {}
    
    int Lock() const;
    int Trylock() const;
    int Unlock() const;
    
private:
    mutable pthread_rwlock_t* wlock_;
};    

////////////////////////////////////////////////////
// @brief wrap read/write lock
class CRWLock 
{
public:
    explicit CRWLock(ELockMode kLockMode = NO_PRIORITY);
    ~CRWLock();

    CRLock* rlock() const {return rlock_;}
    CWLock* wlock() const {return wlock_;} 

private:
    CRLock* rlock_;
    CWLock* wlock_;
    pthread_rwlock_t rwlock_;
};

////////////////////////////////////////////////////
// @brief 对linux线程锁中的读写锁封装
class CRWSimpleLock
{
public:
    CRWSimpleLock(ELockMode lockMode = NO_PRIORITY);
    ~CRWSimpleLock();
    
    int ReadLock();
    int WriteLock();
    int TryReadLock();
    int TryWriteLock();
    int Unlock();
    
private:    
    pthread_rwlock_t rwlock_;
};

////////////////////////////////////////////////////
// @brief linux 线程锁中读锁的助手类
class CRLockGuard
{
public:
    CRLockGuard(const CRWLock& rwlock, bool block = true) : guard_((*rwlock.rlock()), block) {}
    ~CRLockGuard(){}

    bool acquired()
    {
        return guard_.acquired();
    }

private:
    LockGuard<CRLock> guard_;
};

////////////////////////////////////////////////////
// @brief linux 线程锁中写锁的助手类
class CWLockGuard
{
public:
    CWLockGuard(const CRWLock& rwlock, bool block = true) : guard_((*rwlock.wlock()), block) {}
    ~CWLockGuard(){}

    bool acquired()
    {
        return guard_.acquired();
    }

private:
    LockGuard<CWLock> guard_;
};

}   // end of namespace
#endif

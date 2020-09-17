/**
 * Copyright 2015 kingsoft.com Inc. All rights reserved.
 *
 * @file client/scoped_locker.h
 * @author zhouwei
 * @date 2015/04/27 11:48:55
 * @brief
 *
 **/

#ifndef _KS3_SCOPED_LOCKER_H_
#define _KS3_SCOPED_LOCKER_H_

#include <pthread.h>

namespace ks3 {
namespace sdk {

class MutexLock {
public:
    MutexLock() {
        pthread_mutex_init(&lock_, NULL);
    }

    ~MutexLock() {
        pthread_mutex_destroy(&lock_);
    }

    void Lock() {
        pthread_mutex_lock(&lock_);
    }

    void Unlock() {
        pthread_mutex_unlock(&lock_);
    }

private:
    pthread_mutex_t lock_;
};

class RWLock {
public:
    RWLock() {
        pthread_rwlock_init(&lock_, NULL);
    }

    ~RWLock() {
        pthread_rwlock_destroy(&lock_);
    }

    void Lock() {
        pthread_rwlock_wrlock(&lock_);
    }

    void SharedLock() {
        pthread_rwlock_rdlock(&lock_);
    }

    void Unlock() {
        pthread_rwlock_unlock(&lock_);
    }

private:
    pthread_rwlock_t lock_;
};

template <typename LockType>
class ScopedLocker {
public:
    //explicit
    ScopedLocker(LockType& lock) : lock_(&lock) {
        lock_->Lock();
    }

    ScopedLocker(LockType* lock) : lock_(lock) {
        lock_->Lock();
    }

    ~ScopedLocker() {
        lock_->Unlock();
    }

private:
    LockType* lock_;
};

class ScopedReadLocker {
public:
    explicit ScopedReadLocker(RWLock& lock) : lock_(&lock) {
        lock_->SharedLock();
    }

    explicit ScopedReadLocker(RWLock* lock) : lock_(lock) {
        lock_->SharedLock();
    }

    ~ScopedReadLocker() {
        lock_->Unlock();
    }

private:
    RWLock* lock_;
};

class ScopedWriteLocker {
public:
    explicit ScopedWriteLocker(RWLock& lock) : lock_(&lock) {
        lock_->Lock();
    }

    explicit ScopedWriteLocker(RWLock* lock) : lock_(lock) {
        lock_->Lock();
    }

    ~ScopedWriteLocker() {
        lock_->Unlock();
    }

private:
    RWLock* lock_;
};

}
}

#endif

/* vim: set ts=4 sw=4 sts=4 tw=100 */

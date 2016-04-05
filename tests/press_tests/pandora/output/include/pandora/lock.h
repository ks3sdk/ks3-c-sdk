/*
 * @landyliu
*/

#ifndef PANDORA_LOCK_H
#define PANDORA_LOCK_H

#include <assert.h>

namespace pandora 
{
/*
 * LockT is a simple template class
 * Constructor lock mutex, Destructor unlock mutex
 * Make a part LockT variable to use it.
 */
template <typename T>
class LockT
{
public:
    
    LockT(const T& mutex) :
        mutex_(mutex)
    {
        mutex_.Lock();
        acquired_ = true;
    }

    ~LockT()
    {
        if (acquired_)
        {
            mutex_.Unlock();
        }
    }
    
    void Acquire() const
    {
        if (acquired_)
        {
           assert(!"ThreadLockedException");
        }
        mutex_.Lock();
        acquired_ = true;
    }


    bool TryAcquire() const
    {
        if (acquired_)
        {
            assert(!"ThreadLockedException");
        }
        acquired_ = mutex_.TryLock();
        return acquired_;
    }

    void Release() const
    {
        if (!acquired_)
        {
            assert(!"ThreadLockedException");
        }
        mutex_.Unlock();
        acquired_ = false;
    }

    bool Acquired() const
    {
        return acquired_;
    }
   
protected:
    
    LockT(const T& mutex, bool) :
        mutex_(mutex)
    {
        acquired_ = mutex_.TryLock();
    }

private:
    
    LockT(const LockT&);
    LockT& operator=(const LockT&);

    const T& mutex_;
    mutable bool acquired_;

    friend class Cond;
};

template <typename T>
class TryLockT : public LockT<T>
{
public:

    TryLockT(const T& mutex) :
    LockT<T>(mutex, true)
    {}
};
}  

#endif

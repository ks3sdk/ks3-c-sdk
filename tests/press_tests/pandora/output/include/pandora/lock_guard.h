/*
 * @landyliu
 */

#ifndef PANDORA_SYSTEM_LOCK_LOCK_GUARD_H_ 
#define PANDORA_SYSTEM_LOCK_LOCK_GUARD_H_ 1 

namespace pandora 
{

/**
 * @brief   
 *  LockGuard is a template class, which take CThreadMutex as its template parameter. 
 *  Constructor invoked 'Lock' method, Destructor invoked 'Unlock' method.
 *
 *  REQUIRE: Template parameter class should provide 'Lock' 'Unlock' 'Trylock' method. 
 */
template <class T>
class LockGuard
{
public:
    LockGuard(const T& lock, bool block = true) : lock_(lock)
    {
        acquired_ = !(block ? lock_.Lock() : lock_.Trylock());
    }

    ~LockGuard()
    {
        lock_.Unlock();
    }

    bool acquired() const
    {
        return acquired_;
    }
    
private:
    const T& lock_;
    mutable bool acquired_;
};

}   // end of namespace

#endif

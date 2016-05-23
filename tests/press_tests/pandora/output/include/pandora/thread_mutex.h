/*
 * @landyliu
*/

#ifndef PANDORA_SYSTEM_THREAD_THREAD_MUTEX_H
#define PANDORA_SYSTEM_THREAD_THREAD_MUTEX_H

#include <pthread.h>
#include <assert.h>

namespace pandora 
{

////////////////////////
class CThreadMutex 
{
public:
    CThreadMutex();
    ~CThreadMutex();

    void Lock();
    int Trylock();
    void Unlock();

protected:
    pthread_mutex_t mutex_;
};

////////////////////////////
class CThreadGuard
{
public:
    explicit CThreadGuard(CThreadMutex * thread_mutex);
    ~CThreadGuard();

private:
    CThreadMutex * thread_mutex_;
};

}   //end of namespace

#endif 

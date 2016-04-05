/*
 * @landyliu
*/

#ifndef PANDORA_SYSTEM_THREAD_CTHREAD_H_
#define PANDORA_SYSTEM_THREAD_CTHREAD_H_ 1 

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <pthread.h>
#include "time_wrap.h"

namespace pandora
{

class CThread;

//thread implement interface
class Runnable 
{
public:
    virtual ~Runnable(){}
    virtual void Run(CThread * thread, void * arg) = 0;
};

// thread controller 
class CThread 
{
public:
    CThread();

    // start and stop thread
    int Start(Runnable * r, void * a) ;
    int Join();

    Runnable * runnable() const { return runnable_; }
    void * args() const { return args_; }
    pthread_t tid() const { return tid_; }

    pthread_t GetThreadId() const;

    // Suspend the thread, and time is decided by param timeout
    static void Ssleep(const pandora::Time& timeout);

    
    // call back function of thread
    static void * Hook(void * arg);

private:
    pthread_t tid_;      
    Runnable * runnable_;
    void * args_;
};

}   //end of namespace 
#endif 

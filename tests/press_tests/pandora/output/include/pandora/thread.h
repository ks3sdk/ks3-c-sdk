/*
 * @landyliu
*/

#ifndef PANDORA_THREAD_H
#define PANDORA_THREAD_H 

#include "shared.h"
#include "handle.h"
#include "mutex.h"
#include "cond.h"
#include "time_wrap.h"
#include "log.h"

namespace pandora
{
/*
 * Thread class is a template class.
 * It has a virtual method Run.
 * You must extends Thread and impl the Run fun to use .
 */
class Thread : virtual public pandora::Shared
{
public:

    Thread();
    virtual ~Thread();

    virtual void Run() = 0;

    // Create a new thread in this fun
    int  Start(size_t stack_size= 0);
    bool IsAlive() const; 
    void Done(); 
    int Join();  
    int Detach();
    pthread_t thread_id() const;
    static void Yield();
    static void Ssleep(const pandora::Time& timeout);

protected:
    bool running_;    // whether the thread is running
    bool started_;    // whether the thread is started
    bool detachable_; 
    pthread_t thread_id_;
    pandora::Mutex mutex_;    
private:
    Thread(const Thread&);            
    Thread& operator=(const Thread&);   
};
typedef pandora::Handle<Thread> ThreadPtr;
}//end namespace pandora

#endif


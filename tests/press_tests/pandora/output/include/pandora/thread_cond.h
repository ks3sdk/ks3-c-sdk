/*
 * @landyliu
*/

#ifndef PANDORA_SYSTEM_THREAD_THREAD_COND_H
#define PANDORA_SYSTEM_THREAD_THREAD_COND_H

#include <sys/time.h>
#include <stdint.h>
#include "thread_mutex.h"

namespace pandora 
{
    
class CThreadCond : public CThreadMutex 
{
public:
    CThreadCond();
    ~CThreadCond();

    /**
     * @brief wait for the signal
     * @param milliseconds : wait timeout(ms), 0 = wait forever
     */
    bool Wait(int milliseconds = 0);
    // wake up one
    void Signal();
    // wake up all
    void Broadcast();

private:
    pthread_cond_t cond_;
};

} // end of namespace 
#endif 

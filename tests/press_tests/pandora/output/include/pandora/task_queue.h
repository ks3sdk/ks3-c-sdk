/************************************************************
 *
 * Author: @landyliu
 *
 * Description:
 *  1) TaskNode used with AmFrame, store received packet and handler
 *  2) TaskQueue basede on stl queue, multi-thread safe and
 *      with semophore signal support
 *
 * Version: 1.0
 *************************************************************/

#ifndef PANDORA_INCLUDE_TASK_QUEUE_H_
#define PANDORA_INCLUDE_TASK_QUEUE_H_

#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <queue>

 // namespace pandora
 // {
 // class StreamSocketHandler;
 // }

namespace pandora
{
 // using pandora::StreamSocketHandler;
 // /* TaskNode used to copy content from AmFrame's packet
 //  * when used with taskqueue */
 // struct TaskNode {
 //     explicit TaskNode(size_t n) : length(n), channel_id(0)
 //     {
 //         content = new uint8_t[n];
 //         socket_handler = NULL;
 //         client_data = NULL;
 //     }
 // 
 //     ~TaskNode()
 //     {
 //         if (content)
 //             delete []content;
 //         length = 0;
 //     }
 // 
 //     uint8_t * content;
 //     size_t length;
 // 
 //     // maintain the status fro amframe:
 //     StreamSocketHandler * socket_handler;
 //     uint32_t channel_id;
 //     void *client_data;
 //     uint64_t unpack_time;
 // };

template<typename T>
class TaskQueue
{
public:
    TaskQueue() : max_node_number_(0) {}
    ~TaskQueue() {}
    bool Init(uint32_t max_node_number);

    /**
     * wait until success
     */
    bool WaitTillPush(const T& node);
    bool WaitTillPop(T& Node);

    /**
     * wait for usec, 0 for success and other for error
     */
    bool WaitTimePush(const T& Node, uint32_t usec);
    bool WaitTimePop(T& Node, uint32_t usec);

    bool IsEmpty() const;

    uint32_t Size() const;

    uint32_t max_node_number() const
    {
        return max_node_number_;
    }

private:
    std::queue<T> queue_;
    sem_t exist_nodes_;
    sem_t empty_nodes_;
    pthread_mutex_t lock_;
    uint32_t max_node_number_;
};

template<typename T>
bool TaskQueue<T>::Init(uint32_t n)
{
    int result = 0;
    max_node_number_ = n;
    result = sem_init(&exist_nodes_, 0, 0);
    if (result == -1)
        return false;
    result = sem_init(&empty_nodes_, 0, max_node_number_);
    if (result == -1)
        return false;
    result = pthread_mutex_init(&lock_, NULL);
    if (result == -1)
        return false;
    return true;
}

template<typename T>
bool TaskQueue<T>::WaitTillPush(const T& node)
{
    int flag = 0;
    while (1) {
        flag = sem_wait(&empty_nodes_);
        if (-1 == flag) {
            if (errno == EINTR)
                continue;
            else
                return false;
        } else {
            break;
        }
    }
    flag = pthread_mutex_lock(&lock_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    queue_.push(node);
    flag = sem_post(&exist_nodes_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    flag = pthread_mutex_unlock(&lock_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    return true;
}

template<typename T>
bool TaskQueue<T>::WaitTillPop(T& node)
{
    int flag = 0;
    while (1) {
        flag = sem_wait(&exist_nodes_);
        if (-1 == flag) {
            if (errno == EINTR)
                continue;
            else
                return false;
        } else {
            break;
        }
    }
    flag = pthread_mutex_lock(&lock_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    node = queue_.front();
    queue_.pop();
    flag = sem_post(&empty_nodes_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    flag = pthread_mutex_unlock(&lock_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    return true;
}

template<typename T>
bool TaskQueue<T>::WaitTimePush(const T& node,
                                uint32_t usec)
{
    struct timespec ts;
    struct timeval tt;
    gettimeofday(&tt, NULL);
    ts.tv_sec = tt.tv_sec;
    ts.tv_nsec = tt.tv_usec * 1000 + usec * 1000 * 1000;
    ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
    ts.tv_nsec %= (1000 * 1000 * 1000);
    int flag = 0;
    while (1) {
        flag = sem_timedwait(&empty_nodes_, &ts);
        if (-1 == flag) {
            if (errno == EINTR) {
                continue;
            } else if (errno == ETIMEDOUT) {
                return false;
            } else {
                return false;
            }
        } else {
            break;
        }
    }
    flag = pthread_mutex_lock(&lock_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    queue_.push(node);
    flag = sem_post(&exist_nodes_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    flag = pthread_mutex_unlock(&lock_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    return true;
}

template<typename T>
bool TaskQueue<T>::WaitTimePop(T& node, uint32_t usec)
{
    struct timespec ts;
    struct timeval tt;
    gettimeofday(&tt, NULL);
    ts.tv_sec = tt.tv_sec;
    ts.tv_nsec = tt.tv_usec * 1000 + usec * 1000 * 1000;
    ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
    ts.tv_nsec %= (1000 * 1000 * 1000);
    int flag = 0;
    while (1) {
        flag = sem_timedwait(&exist_nodes_, &ts);
        if (-1 == flag) {
            if (errno == EINTR) {
                continue;
            } else if (errno == ETIMEDOUT) {
                return false;
            } else {
                return false;
            }
        } else {
            break;
        }
    }
    flag = pthread_mutex_lock(&lock_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    node = queue_.front();
    queue_.pop();
    flag = sem_post(&empty_nodes_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    flag = pthread_mutex_unlock(&lock_);
    if (flag == -1) {
        pthread_mutex_unlock(&lock_);
        return false;
    }
    return true;
}

template<typename T>
bool TaskQueue<T>::IsEmpty() const
{
    return queue_.empty();
}

template<typename T>
uint32_t TaskQueue<T>::Size() const
{
    return (uint32_t) queue_.size();
}

}  // namespace pandora

#endif  // PANDORA_INCLUDE_TASK_QUEUE_H_

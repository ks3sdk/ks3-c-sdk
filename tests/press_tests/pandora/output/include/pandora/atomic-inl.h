/**
 * @landyliu
 */

#ifndef PANDORA_SYSTEM_SYSTEM_ATOMIC_ATOMIC_INL_H_
#define PANDORA_SYSTEM_SYSTEM_ATOMIC_ATOMIC_INL_H_ 1 

#define LOCK "lock ; "

namespace pandora 
{

typedef struct 
{
    volatile int32_t counter32;
} atomic_t;

typedef struct 
{
    volatile int64_t counter64;
} atomic64_t;

__inline__ int32_t AtomicGet(atomic_t *v)
{
    return v->counter32;
}

__inline__ int64_t AtomicGet64(atomic64_t *v)
{
    return v->counter64;
}

__inline__ void AtomicSet(atomic_t *v, int32_t i)
{
    v->counter32 = i;
}

__inline__ void AtomicSet64(atomic64_t *v, int64_t i)
{
    v->counter64 = i;
}

__inline__ int32_t AtomicAdd(atomic_t *v, int32_t i)
{
    int32_t __i;
    __i = i;
    __asm__ __volatile__(
        LOCK "xaddl %0, %1"
        :"+r" (i), "+m" (v->counter32)
        : : "memory");
    return i + __i;
}

__inline__ int64_t AtomicAdd64(atomic64_t *v, int64_t i)
{
    int64_t __i;
    __i = i;
    __asm__ __volatile__(
        LOCK "xaddq %0, %1"
        :"+r" (i), "+m" (v->counter64)
        : : "memory");
    return i + __i;
}

__inline__ int32_t AtomicSub(atomic_t *v, int32_t i)
{
    return AtomicAdd(v, -i);
}

__inline__ int64_t AtomicSub64(atomic64_t *v, int64_t i)
{
    return AtomicAdd64(v, -i);
}

__inline__ void AtomicInc(atomic_t *v)
{
    __asm__ __volatile__(
        LOCK "incl %0"
        :"=m" (v->counter32)
        :"m" (v->counter32));
}

__inline__ void AtomicInc64(atomic64_t *v)
{
    __asm__ __volatile__(
        LOCK "incq %0"
        :"=m" (v->counter64)
        :"m" (v->counter64));
}

__inline__ void AtomicDec(atomic_t *v)
{
    __asm__ __volatile__(
        LOCK "decl %0"
        :"=m" (v->counter32)
        :"m" (v->counter32));
}

__inline__ void AtomicDec64(atomic64_t *v)
{
    __asm__ __volatile__(
        LOCK "decq %0"
        :"=m" (v->counter64)
        :"m" (v->counter64));
}

__inline__ int32_t AtomicExchange(atomic_t *v, int32_t i)
{
    int32_t result;

    __asm__ __volatile__(
        "xchgl %0, %1"
        :"=r"(result)
        :"m"(v->counter32), "0"(i)
        :"memory");
    return result;
}

__inline__ int64_t AtomicExchange64(atomic64_t *v, int64_t i)
{
    int64_t result;

    __asm__ __volatile__(
        "xchgq %0, %1"
        :"=r"(result)
        :"m"(v->counter64), "0"(i)
        :"memory");
    return result;
}

} // end of namespace
#endif

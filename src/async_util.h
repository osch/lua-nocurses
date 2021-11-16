#include "util.h"

#ifndef NOCURSES_ASYNC_UTIL_H
#define NOCURSES_ASYNC_UTIL_H

/* -------------------------------------------------------------------------------------------- */

#define async_util_abort nocurses_async_util_abort
bool async_util_abort(int rc, int line); /* function does not return */

/* -------------------------------------------------------------------------------------------- */

#if defined(NOCURSES_ASYNC_USE_WIN32)
typedef LONG  AtomicCounter;
typedef PVOID AtomicPtr;
#elif defined(NOCURSES_ASYNC_USE_STDATOMIC)
typedef atomic_int      AtomicCounter;
typedef atomic_intptr_t AtomicPtr;
#elif defined(NOCURSES_ASYNC_USE_GNU)
typedef int   AtomicCounter;
typedef void* AtomicPtr;
#endif

/* -------------------------------------------------------------------------------------------- */

static inline bool atomic_set_ptr_if_equal(AtomicPtr* ptr, void* oldPtr, void* newPtr)
{
#if defined(NOCURSES_ASYNC_USE_WIN32)
    return oldPtr == InterlockedCompareExchangePointer(ptr, newPtr, oldPtr);
#elif defined(NOCURSES_ASYNC_USE_STDATOMIC)
    intptr_t old = (intptr_t) oldPtr;
    return atomic_compare_exchange_strong(ptr, &old, (intptr_t)newPtr);
#elif defined(NOCURSES_ASYNC_USE_GNU)
    return __sync_bool_compare_and_swap(ptr, oldPtr, newPtr);
#endif
}

static inline void* atomic_get_ptr(AtomicPtr* ptr)
{
#if defined(NOCURSES_ASYNC_USE_WIN32)
    return InterlockedCompareExchangePointer(ptr, 0, 0);
#elif defined(NOCURSES_ASYNC_USE_STDATOMIC)
    return (void*)atomic_load(ptr);
#elif defined(NOCURSES_ASYNC_USE_GNU)
    return __sync_add_and_fetch(ptr, 0);
#endif
}

/* -------------------------------------------------------------------------------------------- */

static inline int atomic_inc(AtomicCounter* value)
{
#if defined(NOCURSES_ASYNC_USE_WIN32)
    return InterlockedIncrement(value);
#elif defined(NOCURSES_ASYNC_USE_STDATOMIC)
    return atomic_fetch_add(value, 1) + 1;
#elif defined(NOCURSES_ASYNC_USE_GNU)
    return __sync_add_and_fetch(value, 1);
#endif
}

static inline int atomic_dec(AtomicCounter* value)
{
#if defined(NOCURSES_ASYNC_USE_WIN32)
    return InterlockedDecrement(value);
#elif defined(NOCURSES_ASYNC_USE_STDATOMIC)
    return atomic_fetch_sub(value, 1) - 1;
#elif defined(NOCURSES_ASYNC_USE_GNU)
    return __sync_sub_and_fetch(value, 1);
#endif
}

static inline bool atomic_set_if_equal(AtomicCounter* value, int oldValue, int newValue)
{
#if defined(NOCURSES_ASYNC_USE_WIN32)
    return oldValue == InterlockedCompareExchange(value, newValue, oldValue);
#elif defined(NOCURSES_ASYNC_USE_STDATOMIC)
    return atomic_compare_exchange_strong(value, &oldValue, newValue);
#elif defined(NOCURSES_ASYNC_USE_GNU)
    return __sync_bool_compare_and_swap(value, oldValue, newValue);
#endif
}

static inline int atomic_get(AtomicCounter* value)
{
#if defined(NOCURSES_ASYNC_USE_WIN32)
    return InterlockedCompareExchange(value, 0, 0);
#elif defined(NOCURSES_ASYNC_USE_STDATOMIC)
    return atomic_load(value);
#elif defined(NOCURSES_ASYNC_USE_GNU)
    return __sync_add_and_fetch(value, 0);
#endif
}

static inline int atomic_set(AtomicCounter* value, int newValue)
{
#if defined(NOCURSES_ASYNC_USE_WIN32)
    return InterlockedExchange(value, newValue);
#elif defined(NOCURSES_ASYNC_USE_STDATOMIC)
    return atomic_exchange(value, newValue);
#elif defined(NOCURSES_ASYNC_USE_GNU)
    int rslt = __sync_lock_test_and_set(value, newValue);
    __sync_synchronize();
    return rslt;
#endif
}

/* -------------------------------------------------------------------------------------------- */


#endif /* NOCURSES_ASYNC_UTIL_H */


#ifndef NOCURSES_ASYNC_DEFINES_H
#define NOCURSES_ASYNC_DEFINES_H

/* -------------------------------------------------------------------------------------------- */

#if    defined(NOCURSES_ASYNC_USE_WIN32) \
    && (   defined(NOCURSES_ASYNC_USE_STDATOMIC) \
        || defined(NOCURSES_ASYNC_USE_GNU))
  #error "NOCURSES_ASYNC: Invalid compile flag combination"
#endif
#if    defined(NOCURSES_ASYNC_USE_STDATOMIC) \
    && (   defined(NOCURSES_ASYNC_USE_WIN32) \
        || defined(NOCURSES_ASYNC_USE_GNU))
  #error "NOCURSES_ASYNC: Invalid compile flag combination"
#endif
#if    defined(NOCURSES_ASYNC_USE_GNU) \
    && (   defined(NOCURSES_ASYNC_USE_WIN32) \
        || defined(NOCURSES_ASYNC_USE_STDATOMIC))
  #error "NOCURSES_ASYNC: Invalid compile flag combination"
#endif
 
/* -------------------------------------------------------------------------------------------- */

#if    !defined(NOCURSES_ASYNC_USE_WIN32) \
    && !defined(NOCURSES_ASYNC_USE_STDATOMIC) \
    && !defined(NOCURSES_ASYNC_USE_GNU)

    #if defined(WIN32) || defined(_WIN32)
        #define NOCURSES_ASYNC_USE_WIN32
    #elif defined(__GNUC__)
        #define NOCURSES_ASYNC_USE_GNU
    #elif __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)
        #define NOCURSES_ASYNC_USE_STDATOMIC
    #else
        #error "NOCURSES_ASYNC: unknown platform"
    #endif
#endif


/* -------------------------------------------------------------------------------------------- */

#if defined(NOCURSES_ASYNC_USE_WIN32)
    #include <windows.h>
#endif
#if defined(NOCURSES_ASYNC_USE_STDATOMIC)
    #include <stdint.h>
    #include <stdatomic.h>
#endif

/* -------------------------------------------------------------------------------------------- */

#if __STDC_VERSION__ >= 199901L
    #include <stdbool.h>
#else
    #if !defined(__GNUC__) || defined(__STRICT_ANSI__)
        #define inline
    #endif 
    #define bool int
    #define true 1
    #define false 0
#endif

/* -------------------------------------------------------------------------------------------- */


#endif /* NOCURSES_ASYNC_DEFINES_H */

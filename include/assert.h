#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(cond, ...) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "%s:%d: ASSERTION (%s) FAILED", \
                    __FILE__, __LINE__, #cond); \
            __VA_OPT__(fprintf(stderr, ": " __VA_ARGS__);) \
            fputc('\n', stderr); \
            abort(); \
        } \
    } while (0)

#endif /* __ASSERT_H__ */

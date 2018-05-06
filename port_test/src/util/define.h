#ifndef DEFINE_H
#define DEFINE_H

#include <assert.h>

#define ASSERT(con) assert(con)

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);                \
    TypeName& operator=(const TypeName&)
#endif

#ifdef __GNUC__
#define Likely(x)       (__builtin_expect(!!(x), 1))
#define Unlikely(x)     (__builtin_expect(!!(x), 0))
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <stdint.h>

typedef uint8_t bool_t;

#define BOOL_TRUE  ((bool_t)1)
#define BOOL_FALSE ((bool_t)0)


typedef uint32_t freq_t;

#define FREQ_HZ  ((freq_t)1)
#define FREQ_KHZ ((freq_t)(FREQ_HZ*1000))
#define FREQ_MHZ ((freq_t)(FREQ_KHZ*1000))


typedef int64_t nanosecs_t;

#define NANOSECS_NSEC ((nanosecs_t)1)
#define NANOSECS_USEC ((nanosecs_t)(NANOSECS_NSEC*1000))
#define NANOSECS_MSEC ((nanosecs_t)(NANOSECS_USEC*1000))
#define NANOSECS_SEC  ((nanosecs_t)(NANOSECS_MSEC*1000))


#endif /* !CORE_TYPES_H */

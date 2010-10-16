#ifndef BIKE_DEFINES_H
#define BIKE_DEFINES_H

#define always_inline __attribute__((always_inline))

#define interrupt_handler __attribute__ ((interrupt("IRQ")))

#define weak_alias(name) __attribute__ ((weak, alias(name)))

#define ctor_function __attribute__ ((constructor))

#endif /* !BIKE_DEFINES_H */

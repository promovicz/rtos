#ifndef CORE_PLATFORM_H
#define CORE_PLATFORM_H

#ifdef PLATFORM_EMULATOR
#include <platform/emulator/platform.h>
#endif

#ifdef PLATFORM_LPC21
#include <platform/lpc21/platform.h>
#endif

#endif /* !CORE_PLATFORM_H */

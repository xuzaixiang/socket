#ifndef CUS_ARCH_H
#define CUS_ARCH_H

#if defined(__i386) || defined(__i386__) || defined(_M_IX86)
#define CUS_ARCH_X86
#define CUS_ARCH_X86_32
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
#define CUS_ARCH_X64
#define CUS_ARCH_X86_64
#elif defined(__arm__)
#define CUS_ARCH_ARM
#elif defined(__aarch64__) || defined(__ARM64__)
#define CUS_ARCH_ARM64
#else
#error "unsupport hardware architecture!"
#endif

#endif //CUS_ARCH_H
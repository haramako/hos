#pragma once

//#include "console.h"

typedef int error_t;

#define ERR_OK 0
#define ERR_MM_NOT_FOUND 10000

/// Page size by byte.
#define PAGE_SIZE 4096

/// Micro-second by nano-second
#define USEC 1e3
/// Mili-second by nano-second
#define MSEC 1e6
/// Second by nano-second
#define SEC 1e9

// in console.h. for logging.
void console_log(int log_level, const char *fmt, ...);
void console_printfn(const char *fmt, ...);

// Log lelvels
#define CONSOLE_LOG_LEVEL_TRACE 0
#define CONSOLE_LOG_LEVEL_INFO 1
#define CONSOLE_LOG_LEVEL_WARN 2
#define CONSOLE_LOG_LEVEL_ERROR 3
#define CONSOLE_LOG_LEVEL_FATAL 4

// Logging unitilities.
#define klog console_printfn
#define ktrace(...) console_log(CONSOLE_LOG_LEVEL_TRACE, __VA_ARGS__)
#define kinfo(...) console_log(CONSOLE_LOG_LEVEL_INFO, __VA_ARGS__)
#define kwarn(...) console_log(CONSOLE_LOG_LEVEL_WARN, __VA_ARGS__)
#define kerror(...) console_log(CONSOLE_LOG_LEVEL_ERROR, __VA_ARGS__)
#define kfatal(...) console_log(CONSOLE_LOG_LEVEL_TRACE, __VA_ARGS__)

void kpanic(const char *fmt, ...) NORETURN;

#define kcheck(must_true, ...) \
	if (!(must_true)) { \
		kpanic("%s:%d: %s", __FILE__, __LINE__, __VA_ARGS__); \
	};

#define kcheck0(must_true) \
	if (!(must_true)) { \
		kpanic("%s:%d: Check failed.", __FILE__, __LINE__); \
	};

const char *humanize_size(uint64_t size);

char to_hex(char n);
char *dump_bytes(void *p_, size_t size);

inline uint64_t int_merge64(uint32_t high, uint32_t low) { return ((uint64_t)low) | ((uint64_t)high << 32); }
inline uint32_t uint64_high(uint64_t n) { return (uint32_t)(n >> 32); }
inline uint32_t uint64_low(uint64_t n) { return (uint32_t)n; }

#define talloc(type) ((type *)calloc(1, sizeof(type)))

void kbreakpoint();
void kshutdown();

#pragma once

#include "console.h"

//
void console_log(int log_level, const char *fmt,
				 ...); // in console.h. for logging.

// Logging unitilities.
#define klog console_printfn
#define ktrace(...) console_log(CONSOLE_LOG_LEVEL_TRACE, __VA_ARGS__)
#define kinfo(...) console_log(CONSOLE_LOG_LEVEL_INFO, __VA_ARGS__)
#define kwarn(...) console_log(CONSOLE_LOG_LEVEL_WARN, __VA_ARGS__)
#define kerror(...) console_log(CONSOLE_LOG_LEVEL_ERROR, __VA_ARGS__)
#define kfatal(...) console_log(CONSOLE_LOG_LEVEL_TRACE, __VA_ARGS__)

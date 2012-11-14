#pragma once

#define LOG_NAME "boomodule"

/* Debug Logging */
#undef DLOG
#ifdef DEBUG_LOG
# define DLOG(fmt, args...) printk(KERN_DEBUG LOG_NAME " (%s:%d): " fmt, __FUNCTION__, __LINE__, ## args)
#else
# define DLOG(fmt, args...)
#endif

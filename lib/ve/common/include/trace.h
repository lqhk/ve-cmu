/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef TRACE_H
#define TRACE_H

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define TRACE_LV_FUNC 10
#define TRACE_LV_LIB 5
#define TRACE_LV_BIN 1

inline void TRACE_IMPL(const char * fmt, va_list ap) {
  char buf[1024];
  vsnprintf(buf, sizeof(buf), fmt, ap);

  time_t now = time(NULL);
  struct tm t;
  localtime_r(&now, &t);
  fprintf(stderr, "%02d/%02d %02d:%02d:%02d  %s",
          //1900 + t.tm_year,
          t.tm_mon + 1,
          t.tm_mday,
          t.tm_hour, t.tm_min, t.tm_sec,
          buf
          );
}

inline int VERBOSE_LV() {
  return ::getenv("VERBOSE") ? atoi(::getenv("VERBOSE")) : 0;
}

inline void SET_VERBOSITY(int v) {
  char buf[1024];
  snprintf(buf, sizeof(buf), "%d", v);
  ::setenv("VERBOSE", buf, 1);
}

inline void __attribute__((format(printf,2,3))) TRACE(int level, const char * fmt, ...) {
  if(VERBOSE_LV()>=level) {
    va_list ap;
    va_start(ap, fmt);
    TRACE_IMPL(fmt, ap);
    va_end(ap);
  }
}

inline void __attribute__((format(printf,4,5))) DIE_IMPL(const char * FILE_, int LINE_, const char * FUNC_, const char * fmt, ...) {
  char buf[1024];
  snprintf(buf, sizeof(buf), "%s                (at %s:%d:%s)\n", fmt, FILE_, LINE_, FUNC_);
  va_list ap;
  va_start(ap, fmt);
  TRACE_IMPL(buf, ap);
  va_end(ap);
}

#define DIE(retval,...) { DIE_IMPL(__FILE__, __LINE__,__PRETTY_FUNCTION__,__VA_ARGS__); exit(retval); }

inline void __attribute__((format(printf,4,5))) FUNC_IMPL(const char * FILE_, int LINE_, const char * FUNC_, const char * fmt, ...) {
  if(VERBOSE_LV() < TRACE_LV_FUNC) {
    return;
  }

  char buf[1024];
  snprintf(buf, sizeof(buf), "%s %s, at %s:%d\n", fmt, FUNC_, FILE_, LINE_);
  va_list ap;
  va_start(ap, fmt);
  TRACE_IMPL(buf, ap);
  va_end(ap);
}

#define FUNC_LOG_BEGIN() FUNC_IMPL(__FILE__, __LINE__,__PRETTY_FUNCTION__,"BEGIN");
#define FUNC_LOG_END() FUNC_IMPL(__FILE__, __LINE__,__PRETTY_FUNCTION__,"END  ");

// stop and invoke the debugger
#if (defined (__i386__) || defined (__x86_64__)) && (defined (__GNUC__))
#  define BREAK_TO_TRAP() { __asm__ __volatile__ ("int $03"); }
#else
#  define BREAK_TO_TRAP() raise(SIGTRAP);
#endif

#define ASSERT(cond,...) { if(!(cond)) { fprintf(stderr, "assertion '%s' failed ", # cond); fprintf(stderr, "at %s:%d: %s", __FILE__, __LINE__, __PRETTY_FUNCTION__); fprintf(stderr, "\n" __VA_ARGS__); fputc('\n', stderr); BREAK_TO_TRAP();} }

#endif

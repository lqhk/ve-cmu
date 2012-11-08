/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef STRINGF_H
#define STRINGF_H

#include <string>
#include <cstdarg>
#include <climits>

namespace Tools {

  inline std::string __attribute__((format(printf,1,2))) stringf(const char * fmt, ...) {
    va_list ap;
    char buf[PATH_MAX];

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    return std::string(buf);
  }

}

#endif //STRINGF_H

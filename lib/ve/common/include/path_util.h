/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef PATH_UTIL_H
#define PATH_UTIL_H

#include <ctype.h>
#include <string>

namespace Util {

  inline std::string basename(const char * filename) {
    std::string path(filename);
    size_t pos = path.find_last_of("/\\");
    if(pos == path.npos) {
      return path;
    } else {
      return path.substr(pos+1);
    }
  }

  inline std::string dirname(const char * filename) {
    std::string path(filename);
    size_t pos = path.find_last_of("/\\");
    if(pos == path.npos) {
      return ".";
    } else {
      return path.substr(0,pos);
    }
  }

  inline int filename2id(const char * filename) {
    std::string base = basename(filename);
    for(unsigned int i=0 ; i<base.length() ; i++) {
      if(isdigit(base[i]) && base[i] != '0') {
        return atoi(&(base[i]));
      }
    }
    return -1;
  }

}

#endif

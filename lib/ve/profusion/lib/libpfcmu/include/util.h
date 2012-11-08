/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef PFCMU_UTIL_H
#define PFCMU_UTIL_H

#include <endian.h>
#include <stdint.h>

#include "libviewplus/PF_EZInterface.h"

namespace PFCMU {

  /** 
   * Embed the timestamp into the first 4 bytes of each image
   * 
   * @param img [in/out] img->timestamp will be embedded to img->imageArray[0], ...
   */
  inline void embed_timestamp(PF_EZImage * img) {
    uint32_t v = htobe32( uint32_t(img->timestamp) );
    for(int i=0 ; i<img->imageNum ; i++) {
      *(reinterpret_cast<uint32_t *>(img->imageArray[i])) = v;
    }
  }

  /** 
   * set the max process priority (root only)
   */
  void set_max_priority();

  /** 
   * make this process run at only a single CPU (prevent CPU switching)
   */
  void fix_cpu();

  const char * prop_enum2str(PF_EZCameraProperty i);
}

#endif // PF_CMU_H

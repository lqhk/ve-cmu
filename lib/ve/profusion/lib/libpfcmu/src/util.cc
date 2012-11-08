/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#define __STDC_FORMAT_MACROS
#define _GNU_SOURCE 1

#include <sched.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include "util.h"
#include "trace.h"

void PFCMU::fix_cpu() {
  DIE(1,"NOT IMPLEMENTED YET\n");
}

void PFCMU::set_max_priority() {
  struct sched_param param;
  if(0!=sched_getparam(0, &param)) {
    perror("sched_getparam curr");
  }
  param.sched_priority = sched_get_priority_max(SCHED_FIFO);
  if(0!=sched_setscheduler(0, SCHED_FIFO, &param)) {
    perror("sched_setparam new");
    fprintf(stderr, "[WARNING] Cannot enable real-time scheduling.\n"
                    "          Otherwise frame-drops may happen.\n"
                    "          Check /etc/security/limits.d/pfcmu.conf or\n"
                    "          run as root.\n");
    return;
  }
  if(0!=sched_getparam(0, &param)) {
    perror("sched_getparam verify");
    fprintf(stderr, "[WARNING] Cannot enable real-time scheduling.\n"
                    "          Otherwise frame-drops may happen.\n"
                    "          Check /etc/security/limits.d/pfcmu.conf or\n"
                    "          run as root.\n");
    return;
  }
}

const char * PFCMU::prop_enum2str(PF_EZCameraProperty i) {
  switch(i) {
  case PF_EZ_CAMERA_BRIGHTNESS:
    return "BRIGHTNESS";
    break;
  case PF_EZ_CAMERA_SHUTTER:
    return "SHUTTER";
    break;
  case PF_EZ_CAMERA_AUTO_EXPOSURE:
    return "AUTO_EXPOSURE";
    break;
  case PF_EZ_CAMERA_GAIN:
    return "GAIN";
    break;
  default:
    return "UNKNOWN";
    break;
  }
}

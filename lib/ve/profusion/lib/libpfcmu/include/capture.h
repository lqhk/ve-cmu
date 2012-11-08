/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
/**
 * @file   capture.h
 * @author Shohei NOBUHARA <nob@i.kyoto-u.ac.jp>
 * @date   Thu Feb 17 19:22:46 2011
 * 
 * @brief  C wrapper of libviewplus
 */
#ifndef PFCMU_CAPTURE_H
#define PFCMU_CAPTURE_H

#include "libviewplus/PF_EZInterface.h"

extern "C" {
  void PFCMU_capture_setup(PF_EZDeviceHandle * handle, unsigned int index, unsigned int fps);

  void PFCMU_capture_start(PF_EZDeviceHandle handle, PF_EZImage * cue[], unsigned int cue_depth);

  void PFCMU_capture_stop(PF_EZDeviceHandle handle, PF_EZImage * cue[], unsigned int cue_depth);

  // negative value = auto
  int PFCMU_set_shutter(PF_EZDeviceHandle handle, double val);

  // negative value = auto
  int PFCMU_set_gain(PF_EZDeviceHandle handle, double val);

  // negative value = auto
  int PFCMU_set_brightness(PF_EZDeviceHandle handle, double val);

  // negative value = auto
  int PFCMU_set_auto_exposure(PF_EZDeviceHandle handle, double val);
}

#endif

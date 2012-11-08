/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#include <cstring>

#include "trace.h"
#include "pfcmu_config.h"
#include "capture.h"
#include "util.h"


static bool get_auto_state_or_die(PF_EZDeviceHandle handle, PF_EZCameraProperty type) {
  int ret;
  bool auto_state=true;
  if(PF_EZ_OK != (ret=PF_EZPropertyGetAutoState(handle, PF_EZ_ALL_CAMERA, type, &auto_state))) {
    DIE(1, "PF_EZPropertyGetAutoState failed. ret=%d, prop=%s\n", ret, PFCMU::prop_enum2str(type));
  }
  return auto_state;
}

static void set_auto_state_or_die(PF_EZDeviceHandle handle, PF_EZCameraProperty type, bool state) {
  int ret;
  if(PF_EZ_OK != (ret=PF_EZPropertySetAutoState(handle, PF_EZ_ALL_CAMERA, type, state))) {
    DIE(1, "PF_EZPropertySetAutoState failed. ret=%d, prop=%s, state=%d\n", ret, PFCMU::prop_enum2str(type), state);
  }

  bool curr_state=get_auto_state_or_die(handle, type);
  if(curr_state != state && type != PF_EZ_CAMERA_AUTO_EXPOSURE) {
    DIE(1, "PF_EZPropertySetAutoState returned OK, but failed to change the state. ret=%d, prop=%s, state=%d \n", ret, PFCMU::prop_enum2str(type), state);
  }
}

static int set_prop_or_die(PF_EZDeviceHandle handle, PF_EZCameraProperty type, double val) {
  int ret = 0;

  if(val < 0) {
    // turn on the AUTO feature
    set_auto_state_or_die(handle, type, true);
    return ret;
  }

  double max_val, min_val;
  if(PF_EZ_OK != (ret=PF_EZPropertyGetValueRange(handle, 0, type, &min_val, &max_val))) {
    DIE(1, "PF_EZPropertyGetValueRange failed. ret=%d, prop=%s\n", ret, PFCMU::prop_enum2str(type));
  }

  if(val < min_val) {
    val = min_val;
  } else if(val > max_val) {
    val = max_val;
  }

  set_auto_state_or_die(handle, type, false);

  if(PF_EZ_OK != (ret=PF_EZPropertySetValue(handle, PF_EZ_ALL_CAMERA, type, val))) {
    DIE(1, "PF_EZPropertySetValue failed. ret=%d, prop=%s, val=%e\n", ret, PFCMU::prop_enum2str(type), val);
  }

  int err_count = 0;
  double curr;
  for(int i=0 ; i<PFCMU::CAMS ; i++) {
    if(PF_EZ_OK != (ret=PF_EZPropertyGetValue(handle, i, type, &curr))) {
      DIE(1, "PF_EZPropertyGetValue failed. ret=%d, prop=%s, cam=%d\n", ret, PFCMU::prop_enum2str(type), i);
    }
    if(curr != val) {
      TRACE(TRACE_LV_LIB, "cam%02d: set %s failed (%f != %f)\n", i, PFCMU::prop_enum2str(type), curr, val);
      err_count++;
    }
  }

  return err_count;
}

void PFCMU_capture_setup(PF_EZDeviceHandle * handle, unsigned int index, unsigned int fps) {
  FUNC_LOG_BEGIN();

  if (PF_EZGetAPIVersion() >= PF_EZ_API_VERSION) {
    TRACE(TRACE_LV_LIB, "DLL version %d OK\n", PF_EZGetAPIVersion());
  } else {
    DIE(1, "DLL version %d is too old (must be >= %d)\n", PF_EZGetAPIVersion(), PF_EZ_API_VERSION);
  }

  TRACE(TRACE_LV_LIB,
        "ProFUSION device num = %d\n",
        PF_EZGetDeviceNum(PF_EZ_DEVICE_ProFUSION_25_CMU));
  TRACE(TRACE_LV_LIB,
        "ProFUSION Open Device = %d\n",
        index);

  switch(fps) {
  case 100:
    if(PF_EZ_OK != PF_EZOpenDevice(PF_EZ_DEVICE_ProFUSION_25_QVGA_CMU, index, handle)) {
      DIE(1, "Cannot open ProFUSION (device=%d) for QVGA/100fps mode.\n", index);
    }
    break;
  case 25:
    if(PF_EZ_OK != PF_EZOpenDevice(PF_EZ_DEVICE_ProFUSION_25_CMU, index, handle)) {
      DIE(1, "Cannot open ProFUSION (device=%d) for VGA/25fps mode.\n", index);
    }
    break;
  default:
    DIE(1, "Cannot open ProFUSION (device=%d) for unknown fps %d.\n", index, fps);
  }

  ASSERT(NULL != *handle);

  FUNC_LOG_END();
}

void PFCMU_capture_start(PF_EZDeviceHandle handle, PF_EZImage * cue[], unsigned int cue_depth) {
  FUNC_LOG_BEGIN();

#if 0
  if(cue_depth > 2) {
    TRACE(1,
          "\n\n[WARNING] The ringbuf of depth %d may crash the kernel driver. Consider depth 1 or 2 instead.\n\n",
          cue_depth);
  }
#endif

  PF_EZCaptureStart(handle);

  // set default parameters

  // Turn on the AUTO brightness (recommended by ViewPLUS).
  // 'brightness' here corresponds to 'black level' of the CMOS sensor
  // that is supposed to be used in AUTO mode.
  set_auto_state_or_die(handle, PF_EZ_CAMERA_BRIGHTNESS, true);

  // set AUTO_EXPOSURE off
  set_auto_state_or_die(handle, PF_EZ_CAMERA_AUTO_EXPOSURE, false);
  set_auto_state_or_die(handle, PF_EZ_CAMERA_SHUTTER, false);
  set_auto_state_or_die(handle, PF_EZ_CAMERA_GAIN, false);

  for(unsigned int i=0 ; i<cue_depth ; i++) {
    if(PF_EZ_OK != PF_EZGetImage(handle, cue[i])) {
      DIE(1, "PF_EZGetImageAsync failed for cue[%d]\n", i);
    }
    int sz = PF_EZGetSGListLength(handle);
    if(sz > PFCMU::MAX_SGDMA_SIZE) {
      fprintf(stderr, "[WARNING] cue[%d] has %d segments for DMA = very likely to produce frame drops.\n", i, sz);
    }
    TRACE(TRACE_LV_LIB, "scatter gathering list length of cue[%d] = %d\n", i, sz);
  }

  for(unsigned int i=0 ; i<cue_depth ; i++) {
    ASSERT(cue[i] != NULL);

    if(PF_EZ_OK != PF_EZGetImageAsync(handle, cue[i])) {
      DIE(1, "PF_EZGetImageAsync failed for cue[%d]\n", i);
    }
    //TRACE(TRACE_LV_LIB, "PF_EZGetImageAsync(%d)\n", i);
  }

  FUNC_LOG_END();
}

void PFCMU_capture_stop(PF_EZDeviceHandle handle, PF_EZImage * cue[], unsigned int cue_depth) {
  FUNC_LOG_BEGIN();

  for(unsigned int i=0 ; i<cue_depth ; i++) {
    // make this image removed from the cue
    PF_EZWaitImage(cue[i], 1);

    // then destroy it
    if(PF_EZ_OK != PF_EZDisposeImage(handle, cue[i])) {
      DIE(1, "PF_EZDisposeImage failed for cue[%d]\n", i);
    }
    cue[i] = NULL;
  }

  PF_EZCaptureStop(handle);

  FUNC_LOG_END();
}

int PFCMU_set_shutter(PF_EZDeviceHandle handle, double val) {
  return set_prop_or_die(handle, PF_EZ_CAMERA_SHUTTER, val);
}

int PFCMU_set_gain(PF_EZDeviceHandle handle, double val) {
  return set_prop_or_die(handle, PF_EZ_CAMERA_GAIN, val);
}

int PFCMU_set_brightness(PF_EZDeviceHandle handle, double val) {
  if(val>=0) {
    TRACE(0,
          "Turn off the AUTO feature for brightness is NOT RECOMMENDED\n"
          "by ViewPLUS, the original manufacturer of ProFUSION-CMU.\n"
          );
  }

  return set_prop_or_die(handle, PF_EZ_CAMERA_BRIGHTNESS, val);
}

int PFCMU_set_auto_exposure(PF_EZDeviceHandle handle, double val) {
  return set_prop_or_die(handle, PF_EZ_CAMERA_AUTO_EXPOSURE, val);
}

#if 0
typedef struct {
  PF_EZCameraProperty type;
  double max_val;
  double min_val;
  double curr_val;
  bool is_auto_enabled;
} PFCMU_prop_t;

typedef struct {
  PFCMU_prop_t brightness;
  PFCMU_prop_t auto_exposure;
  PFCMU_prop_t shutter;
  PFCMU_prop_t gain;
} PFCMU_props_t;

void PFCMU_get_props(PF_EZDeviceHandle handle, PFCMU_props_t * props);
void PFCMU_set_props(PF_EZDeviceHandle handle, const PFCMU_props_t * props);

static void get_curr(PF_EZDeviceHandle handle, PFCMU_prop_t & p, int camera) {
  PF_EZPropertyGetValueRange(handle, camera, p.type,
                             &(p.min_val),
                             &(p.max_val));
  PF_EZPropertyGetAutoState(handle, camera, p.type,
                            &(p.is_auto_enabled));
  PF_EZPropertyGetValue(handle, camera, p.type,
                        &(p.curr_val));
}

static void get_props_one(PF_EZDeviceHandle handle, PFCMU_props_t * props, int camera) {
  props->brightness.type = PF_EZ_CAMERA_BRIGHTNESS;
  props->shutter.type = PF_EZ_CAMERA_SHUTTER;
  props->gain.type = PF_EZ_CAMERA_GAIN;
  props->auto_exposure.type = PF_EZ_CAMERA_AUTO_EXPOSURE;

  get_curr(handle, props->brightness, camera);
  get_curr(handle, props->shutter, camera);
  get_curr(handle, props->gain, camera);
  get_curr(handle, props->auto_exposure, camera);
}

void PFCMU_get_props(PF_EZDeviceHandle handle, PFCMU_props_t * props) {
  get_props_one(handle, props, 0);
}

static void copy_and_adjust_prop(const PFCMU_prop_t & src, PFCMU_prop_t & dst) {
  dst.is_auto_enabled = src.is_auto_enabled;
  dst.curr_val = src.curr_val;

  // suppose dst has a valid value range, not src.
  if(dst.curr_val < dst.min_val) {
    dst.curr_val = dst.min_val;
  }
  if(dst.curr_val > dst.max_val) {
    dst.curr_val = dst.max_val;
  }
}

static void set_value(PF_EZDeviceHandle handle, const PFCMU_prop_t & p) {
  int ret;
  if( PF_EZ_OK != (ret=PF_EZPropertySetAutoState(handle, PF_EZ_ALL_CAMERA, p.type, p.is_auto_enabled)) ) {
    DIE(1, "PF_EZPropertySetAutoState failed, ret=%d\n", ret);
  }
  if( PF_EZ_OK != (ret=PF_EZPropertySetValue(handle, PF_EZ_ALL_CAMERA, p.type, p.curr_val)) ) {
    DIE(1, "PF_EZPropertySetValue failed, ret=%d\n", ret);
  }
}

void PFCMU_set_props(PF_EZDeviceHandle handle, const PFCMU_props_t * props) {
  PFCMU_props_t p;
  PFCMU_get_props(handle, &p);

  copy_and_adjust_prop(props->brightness, p.brightness);
  copy_and_adjust_prop(props->shutter, p.shutter);
  copy_and_adjust_prop(props->gain, p.gain);
  copy_and_adjust_prop(props->auto_exposure, p.auto_exposure);

  set_value(handle, p.brightness);
  set_value(handle, p.shutter);
  set_value(handle, p.gain);
  set_value(handle, p.auto_exposure);

  PFCMU_props_t q;
  PFCMU_get_props(handle, &q);

  for(int i=0 ; i<PFCMU::CAMS ; i++) {
    get_props_one(handle, &q, i);
    if(q.brightness.curr_val != p.brightness.curr_val) {
      TRACE(TRACE_LV_LIB, "cam%02d: set BRIGHTNESS failed (%f != %f)\n", i, q.brightness.curr_val, p.brightness.curr_val);
    }
    if(q.shutter.curr_val != p.shutter.curr_val) {
      TRACE(TRACE_LV_LIB, "cam%02d: set SHUTTER failed (%f != %f)\n", i, q.shutter.curr_val, p.shutter.curr_val);
    }
    if(q.gain.curr_val != p.gain.curr_val) {
      TRACE(TRACE_LV_LIB, "cam%02d: set GAIN failed (%f != %f)\n", i, q.gain.curr_val, p.gain.curr_val);
    }
    if(q.auto_exposure.curr_val != p.auto_exposure.curr_val) {
      TRACE(TRACE_LV_LIB, "cam%02d: set AUTO_EXPOSURE failed (%f != %f)\n", i, q.auto_exposure.curr_val, p.auto_exposure.curr_val);
    }
  }
}
#endif


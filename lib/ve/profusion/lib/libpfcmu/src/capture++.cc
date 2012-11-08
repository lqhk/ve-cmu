/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#include <vector>
#include <map>
#include <cstring>
#include <sstream>
#include <signal.h>
#include "trace.h"

#include "pfcmu_config.h"
#include "capture++.h"
#include "capture.h"
#include "my_memcpy.h"
#include "util.h"

static PFCMU::Capture * s_capture = NULL;
volatile static sig_atomic_t signal_handler_in_progress = 0;

static int sig_safe_print(const char * msg) {
  int msg_len = 0;
  for(int i=0 ; msg[i] != '\0' ; i++) {
    msg_len = i+1;
  }
  int r = write(1, msg, msg_len);
  return msg_len - r;
}

// signal handler to clean up the device
static void signal_handler(int sig) {
  /// @sa "The GNU C Library" 24.4.2 Handlers That Terminate the Process

  // do not let this be interrupted by other signal calls
  if(signal_handler_in_progress) {
    raise(sig);
  }
  signal_handler_in_progress = 1;

  // clean up
  if(s_capture) {
    // we cannot use C libs (printf, strlen, ... here)
    // @sa signal(7)
    sig_safe_print("\n\nInterrpted by a signal!\nTerminating the current capture before calling the default signal handler.\n\n");
    s_capture->stop();
    s_capture = NULL;
  }

  // revert to the original action
  signal(sig, SIG_DFL);
  raise(sig);
}

static void close_device_or_die(PF_EZDeviceHandle * handle) {
  FUNC_LOG_BEGIN();

  if(*handle == NULL) {
    return;
  }

  int ret = PF_EZ_OK;
  if(PF_EZ_OK != (ret = PF_EZCloseDevice(*handle))) {
    DIE(1, "PF_EZCloseDevice failed, ret=%d\n", ret);
  }
  *handle = NULL;

  FUNC_LOG_END();
}

PFCMU::Capture::Capture() : m_handle(NULL), m_image(NULL), m_cue(NULL), m_cue_depth(0), m_cue_curr(0) {
  FUNC_LOG_BEGIN();
  FUNC_LOG_END();
}

PFCMU::Capture::~Capture() {
  FUNC_LOG_BEGIN();

  stop();
  close_device_or_die(&m_handle);
  if(! signal_handler_in_progress) {
    s_capture = NULL;
  }

  FUNC_LOG_END();
}

void PFCMU::Capture::init(unsigned int index, unsigned int fps) {
  FUNC_LOG_BEGIN();

  // strictly speaking, this part shold be locked
  // since "if(s_capture) {} else {s_capture=this;}"
  // is not atomic. or we can implement this as a
  // singleton class.
  if(s_capture) {
    DIE(1, "Another camera is already in use in this process\n");
  } else {
    s_capture = this;
    for(int i=1 ; i<32 ; i++) {
      signal(i, signal_handler);
    }
  }

  PFCMU_capture_setup(&m_handle, index, fps);
  ASSERT(NULL != m_handle);

  int ret = PF_EZ_OK;
  if(PF_EZ_OK != (ret = PF_EZCreateDeviceImage(m_handle, &m_image))) {
    DIE(1, "PF_EZCreateDeviceImage failed, ret=%d\n", ret);
  }
  m_image_size = (m_image->widthStep) * (m_image->height) * PFCMU::CAMS;

  FUNC_LOG_END();
}

int PFCMU::Capture::start(unsigned int cue_depth) {
  FUNC_LOG_BEGIN();

  ASSERT(cue_depth >= 1);

  // allocate cue_depth + 1 images
  std::vector< PF_EZImage * > img;
  int max_size = allocate_pfimages(cue_depth+1, &img);
  if(max_size > PFCMU::MAX_SGDMA_SIZE) {
    DIE(1, "One of the allocated buffers has %d segments for SG-DMA.\n"
           "This is too many, and very likely to result in frame drops.\n"
           "Since this is due to physical memory fragmentations,\n"
           "\n\n\n    Please REBOOT the OS and try again.\n\n\n", max_size); 
  }

  // construct m_cue
  m_cue = (PF_EZImage **)malloc(sizeof(PF_EZImage*)*cue_depth);
  m_cue_depth = cue_depth;
  m_cue_curr = 0;

  if(NULL == m_cue) {
    DIE(1, "cannot allocate memory for cue\n");
  }

  for(unsigned int i=0 ; i<cue_depth ; i++) {
    m_cue[i] = img[i];
  }

  // and m_image
  PF_EZDisposeImage(m_handle, m_image); // release the old one
  m_image = img[cue_depth]; // use size-verified one

  // then start capture
  PFCMU_capture_start(m_handle, m_cue, m_cue_depth);

  FUNC_LOG_END();

  return max_size;
}

void PFCMU::Capture::stop() {
  FUNC_LOG_BEGIN();

  if(m_handle == NULL) {
    return;
  }

  if(m_cue) {
    PFCMU_capture_stop(m_handle, m_cue, m_cue_depth);
    for(unsigned int i=0 ; i<m_cue_depth ; i++) {
      ASSERT(m_cue[i] == NULL);
    }
    free(m_cue);
    m_cue = NULL;
    m_cue_depth = 0;
  }

  if(m_image) {
    int ret = PF_EZ_OK;
    if(PF_EZ_OK != (ret = PF_EZDisposeImage(m_handle, m_image))) {
      DIE(1, "PF_EZDisposeImage failed, ret=%d\n", ret);
    }
    m_image = NULL;
  }

  FUNC_LOG_END();
}

template<typename T>
void swap_ptr(T & x, T & y) {
  T t = x;
  x = y;
  y = t;
}

void PFCMU::Capture::embed_framecount() {
  PFCMU::embed_timestamp(m_image);
}

void PFCMU::Capture::copy_all(void * buf) const {
  my_memcpy<char>(buf, m_image->imageArray[0], memsize());
}

void PFCMU::Capture::copy(void * buf, int camera, int widthStep) const {
  if(widthStep == m_image->widthStep) {
    my_memcpy<char>(buf,
                    m_image->imageArray[camera],
                    memsize_single());
  } else if(widthStep > m_image->widthStep) {
    for(int y=0 ; y<m_image->height ; y++) {
      my_memcpy<char>((char*)buf+widthStep*y,
                      m_image->imageArray[camera]+m_image->widthStep*y,
                      m_image->width);
    }
  } else {
    DIE(1, "Invalid widthStep %d, which is supposed to be %d or more\n", widthStep, m_image->widthStep);
  }
}

PF_EZImage * PFCMU::Capture::grab() {
  FUNC_LOG_BEGIN();

  if(m_cue_depth == 0) {
    DIE(1, "capture is not started yet\n");
  }

  int ret = PF_EZ_OK;

  const timestamp_t ts = m_image->timestamp;
  do {
    // wait for the next image
    if(PF_EZ_OK != (ret = PF_EZWaitImage(m_cue[m_cue_curr], 0))) {
      DIE(1, "Capture Error, ret=%d\n", ret);
    }

    swap_ptr(m_cue[m_cue_curr], m_image);

    // and then queue it again as fast as possible (to surely catch the next image)
    if(PF_EZ_OK != (ret = PF_EZGetImageAsync(m_handle, m_cue[m_cue_curr]))) {
      DIE(1, "PF_EZGetImageAsync returns Error, ret=%d\n", ret);
    }

    // increment the queue
    m_cue_curr = (m_cue_curr+1)%m_cue_depth;
  } while(ts == m_image->timestamp);

  FUNC_LOG_END();

  return m_image;
}

int PFCMU::Capture::set_brightness(double val) {
  return PFCMU_set_brightness(m_handle, val);
}

int PFCMU::Capture::set_shutter(double val) {
  return PFCMU_set_shutter(m_handle, val);
}

int PFCMU::Capture::set_gain(double val) {
  return PFCMU_set_gain(m_handle, val);
}

int PFCMU::Capture::set_auto_exposure(double val) {
  return PFCMU_set_auto_exposure(m_handle, val);
}

struct prop_state_t {
  double val;
  bool autostate;

  void update(PF_EZDeviceHandle handle, PF_EZCameraProperty type, int idx) {
    int ret;

    if(PF_EZ_OK != (ret=PF_EZPropertyGetAutoState(handle, idx, type, &autostate))) {
      DIE(1, "PF_EZPropertyGetAutoState failed. ret=%d, prop=%s\n", ret, PFCMU::prop_enum2str(type));
    }

  if(PF_EZ_OK != (ret=PF_EZPropertyGetValue(handle, idx, type, &val))) {
      DIE(1, "PF_EZPropertyGetValue failed. ret=%d, prop=%s, cam=%d\n", ret, PFCMU::prop_enum2str(type), idx);
    }
  }
};

template<typename T>
static std::string to_json_array(const std::vector<T> & array) {
  std::ostringstream oss;
  oss << "[ ";
  for(unsigned int i=0 ; i<array.size() ; i++) {
    oss << array[i] << ", ";
  }
  oss << "]";
  return oss.str();
}
				   
std::string PFCMU::Capture::to_json() const {
  unsigned int serial=-1;
  PF_EZResult ret;

  if(PF_EZ_OK != (ret = PF_EZDeviceGetSerialNumber(m_handle, &serial))) {
    DIE(1, "PF_EZDeviceGetSerialNumber failed, ret=%d\n", ret);
  }

  char buf[1024];
  snprintf(buf, sizeof(buf),
           "ProFusion-CMU by ViewPLUS, DEVICE#=%u, API=%d",
           serial, PF_EZGetAPIVersion());

  std::ostringstream oss_json;
  oss_json << "\t\"desc\": \"" << buf << "\",\n";

  std::vector<double> val_brightness(PFCMU::CAMS);
  std::vector<double> val_exposure(PFCMU::CAMS);
  std::vector<double> val_shutter(PFCMU::CAMS);
  std::vector<double> val_gain(PFCMU::CAMS);
  std::vector<int> val_brightness_auto(PFCMU::CAMS);
  std::vector<int> val_exposure_auto(PFCMU::CAMS);
  std::vector<int> val_shutter_auto(PFCMU::CAMS);
  std::vector<int> val_gain_auto(PFCMU::CAMS);

  for(int i=0 ; i<PFCMU::CAMS ; i++) {
    prop_state_t p;

    p.update(m_handle, PF_EZ_CAMERA_BRIGHTNESS, i);
    snprintf(buf, sizeof(buf), "  %02d : b=%f(%s)", i, p.val, p.autostate ? "on" : "off");
    val_brightness[i] = p.val;
    val_brightness_auto[i] = p.autostate;
      
    p.update(m_handle, PF_EZ_CAMERA_AUTO_EXPOSURE, i);
    snprintf(buf, sizeof(buf), " ae=%f(%s)", p.val, p.autostate ? "on" : "off");
    val_exposure[i] = p.val;
    val_exposure_auto[i] = p.autostate;

    p.update(m_handle, PF_EZ_CAMERA_SHUTTER, i);
    snprintf(buf, sizeof(buf), " s=%f(%s)", p.val, p.autostate ? "on" : "off");
    val_shutter[i] = p.val;
    val_shutter_auto[i] = p.autostate;

    p.update(m_handle, PF_EZ_CAMERA_GAIN, i);
    snprintf(buf, sizeof(buf), " g=%f(%s)", p.val, p.autostate ? "on" : "off");
    val_gain[i] = p.val;
    val_gain_auto[i] = p.autostate;

  }

  oss_json << "\t\"brightness\": " << to_json_array(val_brightness) << ",\n";
  oss_json << "\t\"brightness_auto\": " << to_json_array(val_brightness_auto) << ",\n";

  oss_json << "\t\"exposure\": " << to_json_array(val_exposure) << ",\n";
  oss_json << "\t\"exposure_auto\": " << to_json_array(val_exposure_auto) << ",\n";

  oss_json << "\t\"shutter\": " << to_json_array(val_shutter) << ",\n";
  oss_json << "\t\"shutter_auto\": " << to_json_array(val_shutter_auto) << ",\n";

  oss_json << "\t\"gain\": " << to_json_array(val_gain) << ",\n";
  oss_json << "\t\"gain_auto\": " << to_json_array(val_gain_auto) << ",\n";

  return oss_json.str();
}

std::string PFCMU::Capture::to_string() const {
  unsigned int serial=-1;
  PF_EZResult ret;

  if(PF_EZ_OK != (ret = PF_EZDeviceGetSerialNumber(m_handle, &serial))) {
    DIE(1, "PF_EZDeviceGetSerialNumber failed, ret=%d\n", ret);
  }

  char buf[1024];
  snprintf(buf, sizeof(buf),
           "ProFusion-CMU by ViewPLUS, DEVICE#=%u, API=%d",
           serial, PF_EZGetAPIVersion());

  std::ostringstream oss;
  oss << buf << "\n";

  for(int i=0 ; i<PFCMU::CAMS ; i++) {
    prop_state_t p;

    p.update(m_handle, PF_EZ_CAMERA_BRIGHTNESS, i);
    snprintf(buf, sizeof(buf), "  %02d : b=%f(%s)", i, p.val, p.autostate ? "on" : "off");
    oss << buf;
      
    p.update(m_handle, PF_EZ_CAMERA_AUTO_EXPOSURE, i);
    snprintf(buf, sizeof(buf), " ae=%f(%s)", p.val, p.autostate ? "on" : "off");
    oss << buf;

    p.update(m_handle, PF_EZ_CAMERA_SHUTTER, i);
    snprintf(buf, sizeof(buf), " s=%f(%s)", p.val, p.autostate ? "on" : "off");
    oss << buf;

    p.update(m_handle, PF_EZ_CAMERA_GAIN, i);
    snprintf(buf, sizeof(buf), " g=%f(%s)", p.val, p.autostate ? "on" : "off");
    oss << buf;

    oss << "\n";
  }

  return oss.str();
}


int PFCMU::Capture::allocate_pfimages(const int n, std::vector<PF_EZImage *> * result, int n_try) {
  FUNC_LOG_BEGIN();

  if(n_try < n) {
    n_try = n*2;
  }

  std::multimap<int, PF_EZImage *> tmp;
  for(int i=0 ; i<n_try ; i++) {
    PF_EZImage * img;
    if(PF_EZ_OK != PF_EZCreateDeviceImage(m_handle, &img)) {
      DIE(1, "PF_EZCreateDeviceImage failed for tmp[%d]\n", i);
    }
    tmp.insert(std::make_pair(get_sgdma_length(img), img)); 
  }
  ASSERT((int)tmp.size() == n_try, "n_try = %d, tmp.size() = %zd\n", n_try, tmp.size());

  TRACE(TRACE_LV_LIB, "Sorting generated %d buffers by SG length...\n", n_try);
  int sz = 0;
  result->resize(n);
  std::multimap<int, PF_EZImage *>::iterator itr = tmp.begin();
  for(int i=0 ; i<n ; i++, itr++) {
    ASSERT(sz <= itr->first, "sz=%d, itr->first=%d\n", sz, itr->first);
    sz = itr->first;
    (*result)[i] = itr->second;
    TRACE(TRACE_LV_LIB, "  buf[%d].sglen = %d\n", i, sz);
  }
  
  for(int i=n ; itr != tmp.end() ; i++, itr++) {
    TRACE(TRACE_LV_LIB, "  buf[%d].sglen = %d (not used)\n", i, itr->first);
    PF_EZDisposeImage(m_handle, itr->second);
  }

  TRACE(TRACE_LV_LIB, "Using the best %d buffers, with max sglen = %d\n", n, sz);

  FUNC_LOG_END();

  return sz;
}


int PFCMU::Capture::get_sgdma_length(PF_EZImage * img) {
  PF_EZGetImage(m_handle, img);
  return PF_EZGetSGListLength(m_handle);
}

void PFCMU::Capture::copy_ds(void * buf, int camera, int widthStep) const {
  const int HW = m_image->width / 2;
  const int HH = m_image->height / 2;

  // Suppose the bayer pattern is
  //
  //   GB
  //   RG
  //

  const unsigned char * src = m_image->imageArray[camera];
  const int WS = m_image->widthStep;

  for(int y=0 ; y<HH ; y++) {
    unsigned char * p = (unsigned char *)buf + widthStep * y;
    const unsigned char * q0 = src + WS * 2 * y;
    const unsigned char * q1 = src + WS * (2 * y + 1);
    for(int x=0 ; x<HW ; x++, p+=3, q0+=2, q1+=2) {
      p[0] = q1[0]; // R
      p[1] = q0[0]; // G
      p[2] = q0[1]; // B
    }
  }
}

static void down_sample_debayer_5x5(unsigned char * dst,
				    const int dst_width,
				    const int dst_height,
				    const int dst_widthStep,
				    const unsigned char * src,
				    const int src_widthStep
				    ) {
  //   GB GB GB GB GB GB
  //   RG RG RG RG RG RG

  for(int y=0 ; y<dst_height ; y++) {
    unsigned char * p = dst + dst_widthStep * y;
    const unsigned char * q0 = src + src_widthStep * (5 * y + (y&1));
    const unsigned char * q1 = src + src_widthStep * (5 * y + 1 + (y&1));
    for(int x=0 ; x<dst_width ; x++) {
      p[0] = q1[0]; // R
      p[1] = q0[0]; // G
      p[2] = q0[1]; // B

      p+=3;
      q0+=(4+(x&1)*2); // +4, +6, +4, +6 ...
      q1+=(4+(x&1)*2);
    }
  }
}

void PFCMU::Capture::copy_thumb(void * buf, int widthStep) const {
  const int dst_width = (m_image->width) / 5;
  const int dst_height = (m_image->height) / 5;

  for(int i=0 ; i<24 ; i++) {
    const int x = i%5;
    const int y = i/5;
    down_sample_debayer_5x5(((unsigned char *)buf) + y * dst_height * widthStep + x * dst_width * 3,
			    dst_width,
			    dst_height,
			    widthStep,
			    m_image->imageArray[i],
			    m_image->widthStep);
  }
}

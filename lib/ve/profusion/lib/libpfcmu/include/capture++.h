/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
/**
 * @file   capture++.h
 * @author Shohei NOBUHARA <nob@i.kyoto-u.ac.jp>
 * @date   Thu Feb 17 19:23:10 2011
 *
 * @brief  C++ wrapper of libviewplus
 */
#ifndef PFCMU_CAPTUREXX_H
#define PFCMU_CAPTUREXX_H

#include <string>
#include <vector>

#include "pfcmu_config.h"
#include "libviewplus/PF_EZInterface.h"

namespace PFCMU {
  /**
   * C++ wrapper of libviewplus
   */
  class Capture {
  public:
    Capture();
    ~Capture();

    /**
     * Initialize the camera
     *
     * @param camid [in] 0 = /dev/vpcpro0, the first camera
     * @param fps [in] 25 or 100.
     */
    void init(unsigned int camid, unsigned int fps);

    /**
     * Get the num of chunks for "scatter gathering DMA"
     *
     * @param img [in/out] data to be written
     * @return num of chunks. smaller is better.
     */
    int get_sgdma_length(PF_EZImage * img);
    
    /**
     * Allocate image buffers
     *
     * In order to obtain "better" buffers for DMA transmission, this function allocates n_try buffers first, and then selects n bests to return.
     *
     * @param n [in] num of buffers to be allocated
     * @param result [out] pointers to the allocated buffers
     * @param n_try [in] num of buffers to be evaluated. must be greater than n. Passing 0 means n_try = n*2.
     * @retval max num of chunks
     */
    int allocate_pfimages(const int n, std::vector<PF_EZImage *> * result, int n_try=0);

    /**
     * Start transmission
     *
     * This function turns on AUTO for BRIGHTNESS according to
     * the instruction by ViewPLUS. Other parameters are not
     * changed automatically.
     *
     * @param cue_depth [in] num of buffers in the kernel driver (1 = double-bufferring)
     * @return num of chunks. smaller is better.
     */
    int start(unsigned int cue_depth=1);

    /**
     * Get the latest image in the original format
     *
     * @return the pointer to the image
     */
    PF_EZImage * grab();

    /**
     * Get the framecount of the image given by the last grab().
     *
     * @return the framecount embedded in the images
     */
    timestamp_t get_framecount() const {
      return m_image->timestamp;
    }

    /**
     * Embed the framecount to the image given by the last grab().
     */
    void embed_framecount();

    /**
     * Get the framecount of an image embedded by embed_framecount().
     *
     * @return the framecount embedded in the image
     */
    timestamp_t get_embedded_framecount(int idx) const {
      return PFCMU::get_timestamp(m_image->imageArray[idx]);
    }

    /**
     * Copy all the images given by the last grab().
     *
     * @param buf [out] memsize() bytes array to be written
     */
    void copy_all(void * buf) const;

    /**
     * Copy an image given by the last grab().
     *
     * For example IplImage * buf can be used like copy(buf->imageData, 0, buf->widthStep);
     *
     * @param buf [out] memsize_single() bytes array to be written
     * @param camera [in] camera [0:PFCMU::CAMS-1]
     * @param widthStep [in] width step of buf
     */
    void copy(void * buf, int camera, int widthStep) const;

    /**
     * Copy an image given by the last grab() with downsample debayer.
     *
     * @param buf [out] memsize_single_ds() bytes array to be written
     * @param camera [in] camera [0:PFCMU::CAMS-1]
     * @param widthStep [in] width step of buf
     */
    void copy_ds(void * buf, int camera, int widthStep) const;

    /**
     * Copy thumbnail given by the last grab() with downsample debayer.
     *
     * @param buf [out] memsize_single_ds() bytes array to be written
     * @param widthStep [in] width step of buf
     */
    void copy_thumb(void * buf, int widthStep) const;

    /**
     * Stop transmission
     */
    void stop();

    /**
     * Width of each image (640 or 320)
     *
     * @return width in pixels
     */
    int width() const {
      return m_image->width;
    }

    /**
     * Height of each image (480 or 240)
     *
     * @return height in pixels
     */
    int height() const {
      return m_image->height;
    }

    /**
     * Total bytes of single frame (VGAx24 or QVGAx24)
     *
     * @return bytes
     */
    int memsize() const {
      return (m_image->widthStep) * (m_image->height) * PFCMU::CAMS;
    }

    /**
     * Bytes of single camera image (VGA or QVGA)
     *
     * @return bytes
     */
    int memsize_single() const {
      return (m_image->widthStep) * (m_image->height);
    }

    /**
     * Bytes of downsampled single RGB camera image (VGA or QVGA)
     *
     * @return bytes
     */
    int memsize_single_ds() const {
      return (m_image->width) * (m_image->height) * 3 / 4;
    }

    /**
     * Set BRIGHTNESS parameter (not recommended by ViewPLUS)
     *
     * @param val [in] absolute value for BRIGHTNESS
     *
     * @return 0 on success, otherwise on error
     */
    int set_brightness(double val);

    /**
     * Set SHUTTER parameter
     *
     * In this function, AUTO_EXPOSURE will be automatically
     * turned off.
     *
     * @param val [in] absolute value (seconds) for SHUTTER (eg. 0.01 = 10ms)
     *
     * @return 0 on success, otherwise on error
     */
    int set_shutter(double val);

    /**
     * Set GAIN parameter
     *
     * In this function, AUTO_EXPOSURE will be automatically
     * turned off.
     *
     * @param val [in] absolute value (dB) for GAIN (eg. 10 = 10dB)
     *
     * @return 0 on success, otherwise on error
     */
    int set_gain(double val);

    /**
     * Set AUTO_EXPOSURE parameter
     *
     * @param val [in] absolute value for AUTO_EXPOSURE
     *
     * @return 0 on success, otherwise on error
     */
    int set_auto_exposure(double val);

    /**
     * Returns a string which describes the camera (serial num, etc.)
     * 
     * @return string
     */
    std::string to_string() const;

    std::string to_json() const;

  private:
    Capture(const Capture &); // to disable "object copy"

    PF_EZDeviceHandle m_handle;
    PF_EZImage * m_image;
    PF_EZImage ** m_cue;

    unsigned int m_image_size;
    unsigned int m_cue_depth;
    int m_cue_curr;
  };
}

#endif

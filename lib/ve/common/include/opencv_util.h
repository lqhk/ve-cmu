/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef OPENCV_UTIL_H
#define OPENCV_UTIL_H

#include <vector>

#include <cv.h>
#include <highgui.h>


template <typename _channel_t>
inline _channel_t * ipl_iterator(const IplImage * img, int x, int y) {
  return reinterpret_cast<_channel_t *>((img->imageData) + y*(img->widthStep) + sizeof(_channel_t) * x * (img->nChannels));
}

#endif

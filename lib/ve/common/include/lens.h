/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef LENS_H
#define LENS_H

#include <iostream>
#include <fstream>

#include "my_cvundistort.h"

struct incalib_t {
  double f;
  double u0;
  double v0;

  double k1;
  double k2;
  double p1;
  double p2;

  CvMat * mapx;
  CvMat * mapy;

  incalib_t() : mapx(NULL), mapy(NULL) {
  }

  ~incalib_t() {
    clear();
  }

  void clear() {
    if(mapx) {
      cvReleaseMat(&mapx);
      mapx = NULL;
    }
    if(mapy) {
      cvReleaseMat(&mapy);
      mapy = NULL;
    }
  }

  void load(const char * filename) {
    std::ifstream ifs(filename);
    ifs >> f >> f >> f >> u0 >> v0
        >> k1 >> k2 >> p1 >> p2;
    ifs.close();
  }

  void setup(int width, int height) {
    clear();

    mapx = cvCreateMat(height,width,CV_32FC1);
    mapy = cvCreateMat(height,width,CV_32FC1);

    CvMat * A = cvCreateMat(3,3,CV_32FC1);
    CvMat * k = cvCreateMat(1,4,CV_32FC1);
    cvmSet(A, 0, 0, f);
    cvmSet(A, 0, 1, 0);
    cvmSet(A, 0, 2, u0);
    cvmSet(A, 1, 0, 0);
    cvmSet(A, 1, 1, f);
    cvmSet(A, 1, 2, v0);
    cvmSet(A, 2, 0, 0);
    cvmSet(A, 2, 1, 0);
    cvmSet(A, 2, 2, 1);
    cvmSet(k, 0, 0, k1);
    cvmSet(k, 0, 1, k2);
    cvmSet(k, 0, 2, p1);
    cvmSet(k, 0, 3, p2);

    cvInitUndistortMap(A, k, mapx, mapy);

    cvReleaseMat(&A);
    cvReleaseMat(&k);
  }

  void undistort(const IplImage * src, IplImage * dst) const {
    if(mapx && mapy) {
      cvRemap(src, dst, mapx, mapy, CV_INTER_CUBIC+CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
    } else {
      cvCopyImage(src, dst);
    }
  }
};

#endif //LENS_H

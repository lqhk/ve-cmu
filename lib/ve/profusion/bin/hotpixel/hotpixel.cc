/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#include <highgui.h>
#include <cv.h>
#include <vector>
#include <fstream>
#include "libpfcmu/util.h"
#include "libpfcmu/capture++.h"

#include "boost_opt_util.h"
#include "trace.h"
#include "pfcmu_config.h"

void save_hpx(std::ostream & os, const std::vector<IplImage *> & diff) {
  for(int i=0 ; i<PFCMU::CAMS ; i++) {
    for(int y=0 ; y<diff[i]->height; y++) {
      const unsigned char * d = reinterpret_cast<const unsigned char*>(diff[i]->imageData + diff[i]->widthStep * y);
      for(int x=0 ; x<diff[i]->width; x++, d++) {
        if(*d) {
          os << i << "\t" << x << "\t" << y << "\n";
        }
      }
    }
  }
}

int main(int argc, char * argv[]) {
  boost::program_options::options_description cmdline("Command line options");
  cmdline.add_options()
    ("help,h", "show help message")
    ("fps,f",
     boost::program_options::value<unsigned int>(),
     "[MANDATORY] FPS (25 or 100)")
    ("out,o",
     boost::program_options::value<std::string>(),
     "Output .hpx filename (camera.hpx)")
    ("camera,c",
     boost::program_options::value<unsigned int>()->default_value(0),
     "Device ID (0, 1, ...)")
    ;

  boost::program_options::variables_map parameter_map = boost_opt_check(cmdline, argc, argv);

  const unsigned int CAMERA = parameter_map["camera"].as<unsigned int>();
  const unsigned int FPS = parameter_map["fps"].as<unsigned int>();
  const std::string OUT_FNAME = boost_opt_string(parameter_map, "out");
  const int WIDTH = (FPS == 100 ? 320 : 640);
  const int HEIGHT = (FPS == 100 ? 240 : 480);

  PFCMU::Capture capture;
  capture.init(CAMERA, FPS);
  capture.start();
  capture.set_gain(0);
  capture.set_shutter(30);

  std::vector<IplImage *> diff(PFCMU::CAMS);
  std::vector<IplImage *> prev(PFCMU::CAMS);
  for(int i=0 ; i<PFCMU::CAMS ; i++) {
    diff[i] = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
    prev[i] = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
    cvZero(diff[i]);
    cvAddS(diff[i], cvScalar(255), diff[i]);
  }
  IplImage * buf = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
  IplImage * bgr = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);

  // initial images
  capture.grab();
  for(int i=0 ; i<PFCMU::CAMS ; i++) {
    capture.copy(prev[i]->imageData, i, prev[i]->widthStep);
  }

  // find 'very low' max diff pixels
  int cam = 0;
  CvPoint pt = cvPoint(-1,-1);
  cvNamedWindow("main");
  cvCreateTrackbar("cam", "main", &cam, 23, NULL);
  CvFont font;
  cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 1, CV_AA);
  enum {
    MODE_HOT,
    MODE_FG,
  };
  int mode = MODE_HOT;
  int key = 0;
  int THRESHOLD = 10;
  while( ((key=cvWaitKey(10)) & 0xff) != 'q' ) {
    switch(key&0xff) {
    case 'f':
      mode = mode == MODE_FG ? MODE_HOT : MODE_FG;
      break;
    }

    capture.grab();
    for(int i=0 ; i<PFCMU::CAMS ; i++) {
      capture.copy(buf->imageData, i, buf->widthStep);

      // check diff
      for(int y=0 ; y<HEIGHT ; y++) {
        const unsigned char * p = reinterpret_cast<const unsigned char*>(prev[i]->imageData + prev[i]->widthStep * y);
        const unsigned char * b = reinterpret_cast<const unsigned char*>(buf->imageData + buf->widthStep * y);
        unsigned char * d = reinterpret_cast<unsigned char*>(diff[i]->imageData + diff[i]->widthStep * y);
        for(int x=0 ; x<WIDTH ; x++, p++, b++, d++) {
          if(abs(*p - *b) > THRESHOLD) {
            *d = 0;
          }
        }
      }

      cvCopyImage(buf, prev[i]);
    }

    switch(mode) {
    case MODE_HOT:
      cvCvtColor(diff[cam], bgr, CV_GRAY2BGR);
      break;
    case MODE_FG:
      cvCvtColor(prev[cam], bgr, PFCMU::CV_BAYER2BGR);
      break;
    }

    int n = cvCountNonZero(diff[cam]);
    if(n) {
      char text[1024];
      snprintf(text, sizeof(text), "%d hot pixel%s", n, n==1?"":"s");
      cvPutText(bgr, text, cvPoint(16, HEIGHT-16), &font, CV_RGB(255,0,0));
    } else {
      cvPutText(bgr, "No hot pixels", cvPoint(16, HEIGHT-16), &font, CV_RGB(255,0,0));
    }

    cvShowImage("main", bgr);
  }

  capture.stop();

  if(! OUT_FNAME.empty()) {
    std::ofstream ofs;
    ofs.open(OUT_FNAME.c_str());
    save_hpx(ofs, diff);
  } else {
    save_hpx(std::cout, diff);
  }

  return 0;
}


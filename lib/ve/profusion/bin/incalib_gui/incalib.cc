/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#include <cstdio>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

#include "libpfcmu/capture++.h"
#include "trace.h"
#include "my_memcpy.h"
#include "boost_opt_util.h"
#include "chess_detector.h"
#include "highgui_util.h"
#include "pfcmu_config.h"

struct tbb_findchess {
  std::vector<ChessboardDetector *> & cd;
  std::vector<IplImage *> & buf_mono;
  std::vector<IplImage *> & buf_rgb;
  std::vector<int> & info;
  PF_EZImage * img;

  tbb_findchess(std::vector<ChessboardDetector *> & cd_,
                std::vector<IplImage *> & buf_mono_,
                std::vector<IplImage *> & buf_rgb_,
                std::vector<int> & info_,
                PF_EZImage * img_)
    : cd(cd_), buf_mono(buf_mono_), buf_rgb(buf_rgb_), info(info_), img(img_) {
  }

  void operator()(const tbb::blocked_range<int> & range) const {
    const int MEMSIZE = buf_mono[0]->width * buf_mono[0]->height;
    for(int c=range.begin() ; c!=range.end() ; c++) {
      my_memcpy<char>(buf_mono[c]->imageData, img->imageArray[c], MEMSIZE);
      cvCvtColor(buf_mono[c], buf_rgb[c], CV_BayerGR2BGR);
      cvCvtColor(buf_rgb[c], buf_mono[c], CV_BGR2GRAY);
      int ret = cd[c]->find(buf_mono[c]);
      TRACE(1, "  cam[%02d] ret=%d\n", c, ret);
      cd[c]->draw_indicator(buf_rgb[c], ret);
      cd[c]->draw_chessboard(buf_rgb[c], ret);

      info[c] = ret;
    }
  }
};

struct tbb_incalib {
  std::vector<ChessboardDetector *> & cd;
  std::vector<int> & key_index;
  const CvSize image_size;
  const std::string ofname_fmt;

  tbb_incalib(std::vector<ChessboardDetector *> & cd_,
              std::vector<int> & key_index_,
              const CvSize & image_size_,
              const std::string ofname_fmt_) :
    cd(cd_), key_index(key_index_), image_size(image_size_), ofname_fmt(ofname_fmt_) {
  }

  void operator()(const tbb::blocked_range<int> & range) const {
    std::vector<double> intrinsic_matrix(9);
    std::vector<double> distortion_coeffs(4);
    std::vector<double> translation_vectors;
    std::vector<double> rotation_vectors;
    for(int c=range.begin() ; c!=range.end() ; c++) {
      int Nf = cd[c]->count();
      translation_vectors.resize(Nf*3);
      rotation_vectors.resize(Nf*3);

      CvMat im = cvMat(3, 3, CV_64FC1, &(intrinsic_matrix[0]));
      CvMat dc = cvMat(4, 1, CV_64FC1, &(distortion_coeffs[0]));
      CvMat tv = cvMat(Nf, 3, CV_64FC1, &(translation_vectors[0]));
      CvMat rv = cvMat(Nf, 3, CV_64FC1, &(rotation_vectors[0]));

      cd[c]->calibrate(image_size,
                       &im,
                       &dc,
                       &rv,
                       &tv);


      char buf[65536];
      snprintf(buf, sizeof(buf), ofname_fmt.c_str(), c+1);
      std::ofstream ofs(buf);

      {
        double jacobian[27];
        double rotation[9];
        double rotvec[3] = { rotation_vectors[key_index[c]*3+0],
                             rotation_vectors[key_index[c]*3+1],
                             rotation_vectors[key_index[c]*3+2],
        };
        CvMat mat_j = cvMat(9, 3, CV_64FC1, jacobian);
        CvMat mat_r = cvMat(3, 3, CV_64FC1, rotation);
        CvMat rv = cvMat(1, 3, CV_64FC1, rotvec);
        cvRodrigues2(&rv, &mat_r, &mat_j);

        snprintf(buf, sizeof(buf),
                 "%.16e %.16e %.16e %.16e %.16e\n"
                 "\n"
                 "%.16e %.16e %.16e %.16e\n"
                 "\n"
                 "%e %e %e\n"
                 "%e %e %e\n"
                 "%e %e %e\n"
                 "%e %e %e\n"
                 "\n",
                 intrinsic_matrix[0],
                 intrinsic_matrix[1],
                 intrinsic_matrix[4],
                 intrinsic_matrix[2],
                 intrinsic_matrix[5],
                 distortion_coeffs[0],
                 distortion_coeffs[1],
                 distortion_coeffs[2],
                 distortion_coeffs[3],
                 rotation[0], rotation[1], rotation[2],
                 rotation[3], rotation[4], rotation[5],
                 rotation[6], rotation[7], rotation[8],
                 translation_vectors[key_index[c]*3+0],
                 translation_vectors[key_index[c]*3+1],
                 translation_vectors[key_index[c]*3+2]);
        ofs << buf;
      }
      ofs.close();
    }
  }
};


int main( int argc, char *argv[] ) {
  boost::program_options::options_description cmdline("Command line options");
  cmdline.add_options()
    ("help,h", "show help message")
    ("output,o",
     boost::program_options::value<std::string>(),
     "[MANDATORY] output filename format. (cam%02d.txt)")
    ("camera",
     boost::program_options::value<unsigned int>()->default_value(0),
     "Camera ID (0: first PF, 1: second PF on the PCI bus)")
    ("rows,r",
     boost::program_options::value<unsigned int>()->default_value(4),
     "num of rows of the chessboard corners")
    ("cols,c",
     boost::program_options::value<unsigned int>()->default_value(5),
     "num of cols of the chessboard corners")
    ("size,s",
     boost::program_options::value<double>()->default_value(40),
     "the size of the chess square")
    ("num,n",
     boost::program_options::value<unsigned int>()->default_value(100),
     "num of chessboards to be captured")
    ("skip,S",
     boost::program_options::value<unsigned int>()->default_value(25),
     "use every 'skip' frames only")
    ("window,w",
     boost::program_options::value<double>()->default_value(0.5),
     "size of the debug window. use zero to disable")
    ("shutter",
     boost::program_options::value<double>()->default_value(5),
     "Shutter speed (ms)")
    ("gain",
     boost::program_options::value<double>()->default_value(10),
     "Gain (dB)")
    ;

  boost::program_options::variables_map parameter_map = boost_opt_check(cmdline, argc, argv);

  const unsigned int CAMERA = parameter_map["camera"].as<unsigned int>();
  // チェスボードの行数
  const unsigned int CHESS_ROWS = parameter_map["rows"].as<unsigned int>();
  // チェスボードの列数
  const unsigned int CHESS_COLS = parameter_map["cols"].as<unsigned int>();
  // 実寸
  const double CHESS_SCALE = parameter_map["size"].as<double>();
  // 撮影枚数
  const unsigned int FRAME_NUM = parameter_map["num"].as<unsigned int>();
  // スキップ数
  const unsigned int FRAME_SKIP = parameter_map["skip"].as<unsigned int>();
  // 表示するウィンドウの大きさ
  const double DEBUG_SCALE = parameter_map["window"].as<double>();
  // 出力ファイル
  const std::string OFNAME = parameter_map["output"].as<std::string>();
  const double CAM_SHUTTER = parameter_map["shutter"].as<double>();
  const double CAM_GAIN = parameter_map["gain"].as<double>();

  const int CAMS = PFCMU::CAMS;

  std::vector<ChessboardDetector *> cd(CAMS);
  for(unsigned int i=0 ; i<cd.size() ; i++) {
    cd[i] = new ChessboardDetector(CHESS_ROWS, CHESS_COLS, CHESS_SCALE, FRAME_NUM, 1);
  }

  // GUI を初期化
  cvUseOptimized(0);
  cvInitSystem(argc, argv);
  HighGUI::ScaledWindow window_main("main", DEBUG_SCALE);

  PFCMU::Capture capture;
  capture.init(CAMERA, 25);


  // キャプチャサイズを取得
  const CvSize IMAGE_SIZE = cvSize(capture.width(), capture.height());
  assert(IMAGE_SIZE.width > 0);
  assert(IMAGE_SIZE.height > 0);

  // 中間バッファを用意
  std::vector<IplImage *> buf_rgb(CAMS);
  std::vector<IplImage *> buf_mono(CAMS);
  for(int i=0 ; i<CAMS ; i++) {
    buf_rgb[i] = cvCreateImage( IMAGE_SIZE, IPL_DEPTH_8U, 3 );
    buf_mono[i] = cvCreateImage( IMAGE_SIZE, IPL_DEPTH_8U, 1 );
    assert(buf_mono[i]->widthStep == IMAGE_SIZE.width);
  }

  const int SUB_W = (IMAGE_SIZE.width) / 2;
  const int SUB_H = (IMAGE_SIZE.height) / 2;
  IplImage * buf = cvCreateImage( cvSize(SUB_W*5,SUB_H*5), IPL_DEPTH_8U, 3);

  std::vector<CvRect> roi(CAMS);
  for(int r=0 ; r<5 ; r++) {
    for(int c=0 ; c<5 ; c++) {
      if(r*5+c>=CAMS) { break; }
      roi[r*5+c].x = c * SUB_W;
      roi[r*5+c].y = r * SUB_H;
      roi[r*5+c].width = SUB_W;
      roi[r*5+c].height = SUB_H;
    }
  }
  for(int i=0 ; i<CAMS ; i++) {
    assert(roi[i].width == SUB_W && roi[i].width > 0);
    assert(roi[i].height == SUB_H && roi[i].height > 0);
  }

  std::vector<int> key_index(CAMS, 0);
  std::vector<int> info(CAMS, 0);

  capture.start(1);
  // set params
  TRACE(1, "Camera set auto_exposure\n");
  capture.set_auto_exposure(30);
  TRACE(1, "Camera set shutter = %f\n", CAM_SHUTTER);
  capture.set_shutter(CAM_SHUTTER);
  TRACE(1, "Camera set gain = %f\n", CAM_GAIN);
  capture.set_gain(CAM_GAIN);
  TRACE(1, "Camera set brightness\n");
  capture.set_brightness(0);

  // kill old frames
  for(unsigned int i=0 ; i<10 ; i++) {
    capture.grab();
  }
  TRACE(1, "Now ready to capture\n");

  tbb::task_scheduler_init tbb_init;
  int key_frame_found = 0;
  int curr = 0;
  while( (cvWaitKey(10) & 0xff) != 'q') {
    PF_EZImage * img = capture.grab();

    if( curr % FRAME_SKIP != 0 ) {
      curr ++;
      continue;
    }

    TRACE(1, "timestamp = %08lld\n", img->timestamp);
    if(key_frame_found) {
      cvSet(buf, CV_RGB(0,255,0));
    } else {
      cvSet(buf, CV_RGB(255,255,0));
    }
    
    // multi-thread chess detection
    tbb::parallel_for( tbb::blocked_range<int>(0, CAMS),
                       tbb_findchess(cd, buf_mono, buf_rgb, info, img));

    // generate thumbnail image
    for(int c=0 ; c<CAMS ; c++) {
      cvSetImageROI(buf, roi[c]);
      cvResize(buf_rgb[c], buf);
      cvResetImageROI(buf);
    }
    
    // check if the current frame can be the key frame
    if(key_frame_found == 0) {
      int is_keyframe = 1;
      for(int c=0 ; c<CAMS ; c++) {
        if(info[c] != ChessboardDetector::CHESS_FOUND) {
          is_keyframe = 0;
          break;
        }
      }
      if(is_keyframe) {
        for(int c=0 ; c<CAMS ; c++) {
          key_index[c] = cd[c]->count() - 1;
        }
        key_frame_found = 1;
      }
    }

    window_main.show(buf);
    
    // quit if all done
    int is_all_full = 1;
    for(int c=0 ; c<CAMS ; c++) {
      if(! cd[c]->is_full()) {
        is_all_full = 0;
        break;
      }
    }
    if(is_all_full) break;
  }
  capture.stop();

  // close GUI
  cvDestroyAllWindows();
  cvWaitKey(10);
  for(int i=0 ; i<CAMS ; i++) {
    cvReleaseImage(&buf_rgb[i]);
    cvReleaseImage(&buf_mono[i]);
  }

  // check the detected num
  for(int i=0 ; i<CAMS ; i++) {
    if(cd[i]->count() < 1) {
      fprintf(stderr, "\n\nERROR: No chessboards for cam%02d\n\n", i+1);
      exit(1);
    }
  }

  if(key_frame_found == 0) {
    fprintf(stderr, "\n\nWARNING: No key-frame found\n");
  }

  // multi-thread incalib
  tbb::parallel_for( tbb::blocked_range<int>(0, CAMS),
                     tbb_incalib(cd, key_index, IMAGE_SIZE, OFNAME));


  return 0;
}

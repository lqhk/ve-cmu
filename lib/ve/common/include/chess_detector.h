/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
/**
 * @file   chess_detector.h
 * @author Shohei NOBUHARA
 * @date   Mon Oct 15 19:50:34 2007
 * 
 * @brief  チェスボードを検出するためのルーチン
 * 
 * 
 */
#ifndef CHESS_DETECTOR_H
#define CHESS_DETECTOR_H

#include <cassert>
#include <limits>

#include <cv.h>
#include <highgui.h>

class ChessboardDetector {
public:
  /** 
   * コンストラクタ
   *
   * 実際に検出できたフレームのうち、1/skip フレームを使う。
   *
   * @param rows [in] チェスボードの行方向の交点の数
   * @param cols [in] チェスボードの列方向の交点の数
   * @param scale [in] チェスボードのマスの実寸
   * @param count [in] 検出する数
   * @param skip [in] スキップする数
   */
  ChessboardDetector(int rows, int cols, float scale, unsigned int count, unsigned int skip) :
    m_num_points(rows*cols),
    m_scale(scale),
    m_skip(skip==0 ? 1 : skip),
    m_pattern_size(cvSize(cols, rows)),
    m_corners(NULL),
    m_object_points(NULL),
    m_image_points(NULL),
    m_point_counts(NULL),
    m_count(count),
    m_curr(0) {

    assert(m_skip * m_count < std::numeric_limits<int>::max());

    m_corners = new CvPoint2D32f[m_num_points];
    assert(m_corners);

    m_object_points = new CvPoint3D64d[m_num_points*m_count];
    assert(m_object_points);

    m_image_points = new CvPoint2D64d[m_num_points*m_count];
    assert(m_image_points);

    m_point_counts = new int[m_count];
    assert(m_point_counts);

    for(unsigned int i=0 ; i<count ; i++) {
      m_point_counts[i] = m_num_points;
      int j=0;
      for(int r=0 ; r<rows ; r++){
        for(int c=0 ; c<cols ; c++, j++){
          m_object_points[i*m_num_points + j].x = (c-cols/2)*scale;
          m_object_points[i*m_num_points + j].y = (r-rows/2)*scale;
        }
      }
    }
  }

  ~ChessboardDetector() {
    if(m_corners) delete [] m_corners;
    if(m_object_points) delete [] m_object_points;
    if(m_image_points) delete [] m_image_points;
    if(m_point_counts) delete [] m_point_counts;
  }

  int count() const {
    return m_curr / m_skip;
  }

  int is_full() const {
    return count() == m_count;
  }

  enum {
    CHESS_NOTFOUND = 0,
    CHESS_SKIPPED = 1,
    CHESS_FOUND = 2,
  };

  int find(const IplImage * image) {
    if(is_full()) return CHESS_SKIPPED;

    int corner_count = m_num_points;
    int ret = cvFindChessboardCorners(image, m_pattern_size, m_corners, &corner_count);
    if( ret == 0 || corner_count != m_num_points) {
      return CHESS_NOTFOUND;
    }

    if(m_curr % m_skip != 0) {
      m_curr++;
      return CHESS_SKIPPED;
    }

    cvFindCornerSubPix(image, m_corners, corner_count,
                       cvSize(5, 5), cvSize(-1, -1),
                       cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,10,0.1));
    
    //fprintf(stdout, "# %d\n", m_curr/m_skip);
    for(int i=0 ; i<m_num_points ; i++) {
      m_image_points[m_num_points * (m_curr/m_skip) + i].x = m_corners[i].x;
      m_image_points[m_num_points * (m_curr/m_skip) + i].y = m_corners[i].y;

      //fprintf(stdout, "%f %f\n", m_corners[i].x, m_corners[i].y);
    }
    //fprintf(stdout, "\n");

    m_curr += 1;
    return CHESS_FOUND;
  }

  void draw_chessboard(IplImage * buf, int retval) const {
    if(retval != CHESS_FOUND) return;
    cvDrawChessboardCorners(buf, m_pattern_size, m_corners, m_pattern_size.width * m_pattern_size.height, retval != CHESS_NOTFOUND);
  }

  void draw_indicator(IplImage * buf, int retval) const {
    if(is_full()) {
      draw_mark(buf, CV_RGB(255, 255, 255));
      return;
    }

    switch(retval) {
    case CHESS_NOTFOUND:
      draw_mark(buf, CV_RGB(255, 0, 0));
      break;
    case CHESS_FOUND:
      draw_mark(buf, CV_RGB(0, 255, 0));
      break;
    case CHESS_SKIPPED:
      draw_mark(buf, CV_RGB(255, 255, 0));
      break;
    }
  }

  void calibrate(CvSize image_size, CvMat * intrinsic_matrix, CvMat * distortion_coeffs, CvMat * rotation_vectors=NULL, CvMat * translation_vectors=NULL) const {
    cvSetIdentity(intrinsic_matrix);
    cvmSet(intrinsic_matrix, 0, 0, 1.0);
    cvmSet(intrinsic_matrix, 1, 1, 1.0);
    cvmSet(intrinsic_matrix, 0, 1, 0.0);
    cvmSet(intrinsic_matrix, 0, 2, image_size.width/2.0);
    cvmSet(intrinsic_matrix, 1, 2, image_size.height/2.0);

    const int N = count();
    if(N == 0) {
      return;
    }

    const CvMat object_points = cvMat(N*m_num_points, 1, CV_64FC3, m_object_points);
    const CvMat image_points  = cvMat(N*m_num_points, 1, CV_64FC2, m_image_points);
    const CvMat points_count  = cvMat(N, 1, CV_32SC1, m_point_counts);

    cvCalibrateCamera2(&object_points,
                       &image_points,
                       &points_count,
                       image_size,
                       intrinsic_matrix, distortion_coeffs,
                       rotation_vectors, translation_vectors,
                       CV_CALIB_FIX_ASPECT_RATIO);
  }

private:
  ChessboardDetector(const ChessboardDetector & c);

  const int m_num_points;
  const float m_scale;
  const int m_skip;
  const CvSize m_pattern_size;
  CvPoint2D32f * m_corners;
  CvPoint3D64d * m_object_points;
  CvPoint2D64d * m_image_points;
  int * m_point_counts;
  const int m_count;
  int m_curr;
  
  static void draw_mark(IplImage * buf, CvScalar color) {
    cvRectangle(buf, cvPoint(0,0), cvPoint(buf->width-1, buf->height-1), color, 16, 8, 0);
  }
};


#endif //CHESS_DETECTOR_H

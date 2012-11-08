/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef FINDCHESS_H
#define FINDCHESS_H

#include <cv.h>

int find_chess_corners(IplImage * img_mono, int rows, int cols, IplImage * buf_1u, CvPoint2D32f * corners, int * num_corners, std::vector<double> & result) {
  const CvSize chess_size = { cols, rows };
  const CvSize img_size = cvGetSize(img_mono);
  result.resize(cols*rows*2);

  int ret = cvFindChessBoardCornerGuesses(img_mono, buf_1u, NULL, chess_size, corners, num_corners);

  if(ret == 0 || *num_corners != rows*cols) {
    // 見付からなかった
    for(int i=0 ; i<rows*cols ; i++) {
      result[2*i] = std::numeric_limits<double>::quiet_NaN();
      result[2*i+1] = std::numeric_limits<double>::quiet_NaN();
    }
  } else {
    // 見付かった
    cvFindCornerSubPix(img_mono, corners, *num_corners,
                       cvSize(5, 5), // Half sizes of the search window
                       cvSize(-1, -1), // Half size of the dead region in the middle of the search zone
                       cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10, 0.1));

    for(int i=0 ; i<*num_corners ; i++) {
      result[2*i] = corners[i].x;
      result[2*i+1] = corners[i].y;
    }
  }

  return ret;
}

typedef struct {
  float b_ave_b;
  float b_ave_g;
  float b_ave_r;
  float w_ave_b;
  float w_ave_g;
  float w_ave_r;
} chess_stat_t;

void draw_chess_corners(IplImage * img_rgb, IplImage * img_mono, int rows, int cols, IplImage * buf_1u, chess_stat_t * stat=NULL) {
  static std::vector<double> result;
  static std::vector<CvPoint2D32f> corners;

  const int N = rows * cols;
  result.resize(2*N);
  corners.resize(N);

  int num_corners;
  int ret = find_chess_corners(img_mono, rows, cols, buf_1u, &(corners[0]), &num_corners, result);

  if( ret != 0 && stat != NULL) {
    stat->b_ave_b = 0;
    stat->b_ave_g = 0;
    stat->b_ave_r = 0;
    stat->w_ave_b = 0;
    stat->w_ave_g = 0;
    stat->w_ave_r = 0;

    int num_b = 0;
    int num_w = 0;
    for(int y=0 ; y<rows-1 ; y++) {
      for(int x=0 ; x<cols-1 ; x++) {
        int i = y*cols + x;
        int cx = (corners[i].x + corners[i+1].x + corners[i+cols].x + corners[i+cols+1].x)/4;
        int cy = (corners[i].y + corners[i+1].y + corners[i+cols].y + corners[i+cols+1].y)/4;
        const unsigned char * p = reinterpret_cast<const unsigned char *>(img_rgb->imageData + img_rgb->widthStep * cy) + cx * 3;
        if( (x+y)%2==1 ) {
          stat->w_ave_b += p[0];
          stat->w_ave_g += p[1];
          stat->w_ave_r += p[2];
          num_w+=1;
          cvCircle(img_rgb, cvPoint(cx,cy), 4, CV_RGB(0,0,0));
        } else {
          stat->b_ave_b += p[0];
          stat->b_ave_g += p[1];
          stat->b_ave_r += p[2];
          num_b+=1;
          cvCircle(img_rgb, cvPoint(cx,cy), 4, CV_RGB(255,255,255));
        }
      }
    }

    assert(num_b!=0 && num_w !=0);
    stat->b_ave_b /= num_b;
    stat->b_ave_g /= num_b;
    stat->b_ave_r /= num_b;
    stat->w_ave_b /= num_w;
    stat->w_ave_g /= num_w;
    stat->w_ave_r /= num_w;
  }

  cvDrawChessboardCorners(img_rgb, cvSize(cols, rows), &(corners[0]), num_corners, ret);

}

#endif //FINDCHESS_H

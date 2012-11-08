/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef MY_CVUNDISTORT_H
#define MY_CVUNDISTORT_H

#include <cxmisc.h>
#include <cvinternal.h>

inline void my_cvInitUndistortMap( const CvMat* A, const CvMat* dist_coeffs, CvArr* mapxarr, CvArr* mapyarr ) {
    uchar* buffer = 0;

    CV_FUNCNAME( "cvInitUndistortMap" );

    __BEGIN__;
    
    float a[9], k[4];
    int coi1 = 0, coi2 = 0;
    CvMat mapxstub, *_mapx = (CvMat*)mapxarr;
    CvMat mapystub, *_mapy = (CvMat*)mapyarr;
    float *mapx, *mapy;
    CvMat _a = cvMat( 3, 3, CV_32F, a ), _k;
    int mapxstep, mapystep;
    int u, v;
    float u0, v0, fx, fy, _fx, _fy, k1, k2, p1, p2;
    CvSize size;

    CV_CALL( _mapx = cvGetMat( _mapx, &mapxstub, &coi1 ));
    CV_CALL( _mapy = cvGetMat( _mapy, &mapystub, &coi2 ));

    if( coi1 != 0 || coi2 != 0 )
        CV_ERROR( CV_BadCOI, "The function does not support COI" );

    if( CV_MAT_TYPE(_mapx->type) != CV_32FC1 )
        CV_ERROR( CV_StsUnsupportedFormat, "Both maps must have 32fC1 type" );

    if( !CV_ARE_TYPES_EQ( _mapx, _mapy ))
        CV_ERROR( CV_StsUnmatchedFormats, "" );

    if( !CV_ARE_SIZES_EQ( _mapx, _mapy ))
        CV_ERROR( CV_StsUnmatchedSizes, "" );

    if( !CV_IS_MAT(A) || A->rows != 3 || A->cols != 3  ||
        (CV_MAT_TYPE(A->type) != CV_32FC1 && CV_MAT_TYPE(A->type) != CV_64FC1) )
        CV_ERROR( CV_StsBadArg, "Intrinsic matrix must be a valid 3x3 floating-point matrix" );

    if( !CV_IS_MAT(dist_coeffs) || (dist_coeffs->rows != 1 && dist_coeffs->cols != 1) ||
        dist_coeffs->rows*dist_coeffs->cols*CV_MAT_CN(dist_coeffs->type) != 4 ||
        (CV_MAT_DEPTH(dist_coeffs->type) != CV_64F &&
         CV_MAT_DEPTH(dist_coeffs->type) != CV_32F) )
        CV_ERROR( CV_StsBadArg,
            "Distortion coefficients must be 1x4 or 4x1 floating-point vector" );

    cvConvert( A, &_a );
    _k = cvMat( dist_coeffs->rows, dist_coeffs->cols,
                CV_MAKETYPE(CV_32F, CV_MAT_CN(dist_coeffs->type)), k );
    cvConvert( dist_coeffs, &_k );

    u0 = a[2]; v0 = a[5];
    fx = a[0]; fy = a[4];
    _fx = 1.f/fx; _fy = 1.f/fy;
    k1 = k[0]; k2 = k[1];
    p1 = k[2]; p2 = k[3];

    mapxstep = _mapx->step ? _mapx->step : CV_STUB_STEP;
    mapystep = _mapy->step ? _mapy->step : CV_STUB_STEP;
    mapx = _mapx->data.fl;
    mapy = _mapy->data.fl;

    size = cvGetMatSize(_mapx);
    
    /*if( icvUndistortGetSize_p && icvCreateMapCameraUndistort_32f_C1R_p )
    {
        int buf_size = 0;
        if( icvUndistortGetSize_p( size, &buf_size ) && buf_size > 0 )
        {
            CV_CALL( buffer = (uchar*)cvAlloc( buf_size ));
            if( icvCreateMapCameraUndistort_32f_C1R_p(
                mapx, mapxstep, mapy, mapystep, size,
                a[0], a[4], a[2], a[5], k[0], k[1], k[2], k[3], buffer ) >= 0 )
                EXIT;
        }
    }*/
    
    mapxstep /= sizeof(mapx[0]);
    mapystep /= sizeof(mapy[0]);

    for( v = 0; v < size.height; v++, mapx += mapxstep, mapy += mapystep )
    {
        float y = (v - v0)*_fy;
        float y2 = y*y;
        float _2p1y = 2*p1*y;
        float _2p1y2 = 2*p1*y2;
        float p2y2 = p2*y2;

        for( u = 0; u < size.width; u++ )
        {
            float x = (u - u0)*_fx;
            float x2 = x*x;
            float r2 = x2 + y2;
            float d = 1 + (k1 + k2*r2)*r2;
            float _u = fx*(x*(d + _2p1y) + p2y2 + (3*p2)*x2) + u0;
            float _v = fy*(y*(d + (2*p2)*x) + _2p1y2 + p1*x2) + v0;
            mapx[u] = _u;
            mapy[u] = _v;
        }
    }

    __END__;

    cvFree( &buffer );
}


#endif //MY_CVUNDISTORT_H

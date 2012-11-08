/**
 * @file   mat.inl
 * @author Hiromasa YOSHIMOTO
 * @date   Mon Mar  8 01:47:53 2004
 * 
 * @brief  
 * 
 * 
 */

#if !defined(_MATRIX_H_INCLUDED_)
#define _MATRIX_H_INCLUDED_

#include <vecmath/vm_conf.h>

VM_BEGIN_NS

/** 
 * 
 * 
 * @param mat 
 * @param rfXAngle 
 * @param rfYAngle 
 * @param rfZAngle 
 * 
 * @return 
 */
template <typename T>
bool ToEulerAnglesXYZ (const Matrix3<T>& mat, 
		       T& rfXAngle, T& rfYAngle, T& rfZAngle) 
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    if ( mat.m02 < (T)1.0 )
    {
        if ( mat.m02 > -(T)1.0 )
        {
            rfXAngle = VmUtil<T>::atan2(-mat.m12,mat.m22);
            rfYAngle = VmUtil<T>::asin(mat.m02);
            rfZAngle = VmUtil<T>::atan2(-mat.m01,mat.m00);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            rfXAngle = -VmUtil<T>::atan2(mat.m10,mat.m11);
            rfYAngle = (M_PI*0.5);
            rfZAngle = (T)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        rfXAngle = VmUtil<T>::atan2(mat.m10,mat.m11);
        rfYAngle = (M_PI*0.5);
        rfZAngle = (T)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
/** 
 * 
 * 
 * @param mat 
 * @param rfXAngle 
 * @param rfZAngle 
 * @param rfYAngle 
 * 
 * @return 
 */
template <typename T>
bool ToEulerAnglesXZY (const Matrix3<T>& mat,
		       T & rfXAngle, T& rfZAngle, T& rfYAngle) 
{
    // rot =  cy*cz          -sz              cz*sy
    //        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
    //       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

    if ( mat.m01 < (T)1.0 )
    {
        if ( mat.m01 > -(T)1.0 )
        {
            rfXAngle = VmUtil<T>::atan2(mat.m21,mat.m11);
            rfZAngle = VmUtil<T>::asin(-mat.m01);
            rfYAngle = VmUtil<T>::atan2(mat.m02,mat.m00);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - YA = atan2(r20,r22)
            rfXAngle = VmUtil<T>::atan2(mat.m20,mat.m22);
            rfZAngle = (M_PI*0.5);
            rfYAngle = (T)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XA + YA = atan2(-r20,r22)
        rfXAngle = VmUtil<T>::atan2(-mat.m20,mat.m22);
        rfZAngle = -(M_PI*0.5);
        rfYAngle = (T)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
/** 
 * 
 * 
 * @param mat 
 * @param rfYAngle 
 * @param rfXAngle 
 * @param rfZAngle 
 * 
 * @return 
 */
template <typename T>
bool ToEulerAnglesYXZ (const Matrix3<T>& mat,
		       T& rfYAngle, T& rfXAngle, T& rfZAngle) 
{
    // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
    //        cx*sz           cx*cz          -sx
    //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

    if ( mat.m12 < (T)1.0 )
    {
        if ( mat.m12 > -(T)1.0 )
        {
            rfYAngle = VmUtil<T>::atan2(mat.m02,mat.m22);
            rfXAngle = VmUtil<T>::asin(-mat.m12);
            rfZAngle = VmUtil<T>::atan2(mat.m10,mat.m11);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
            rfYAngle = VmUtil<T>::atan2(mat.m01,mat.m00);
            rfXAngle = (M_PI*0.5);
            rfZAngle = (T)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
        rfYAngle = VmUtil<T>::atan2(-mat.m01,mat.m00);
        rfXAngle = -(M_PI*0.5);
        rfZAngle = (T)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
/** 
 * 
 * 
 * @param mat 
 * @param rfYAngle 
 * @param rfZAngle 
 * @param rfXAngle 
 * 
 * @return 
 */
template <typename T>
bool ToEulerAnglesYZX (const Matrix3<T>& mat,
		       T& rfYAngle, T& rfZAngle, T& rfXAngle) 
{
    // rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
    //        sz              cx*cz          -cz*sx
    //       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

    if ( mat.m10 < (T)1.0 )
    {
        if ( mat.m10 > -(T)1.0 )
        {
            rfYAngle = VmUtil<T>::atan2(-mat.m20,mat.m00);
            rfZAngle = VmUtil<T>::asin(mat.m10);
            rfXAngle = VmUtil<T>::atan2(-mat.m12,mat.m11);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - XA = -atan2(r21,r22);
            rfYAngle = -VmUtil<T>::atan2(mat.m21,mat.m22);
            rfZAngle = -(M_PI*0.5);
            rfXAngle = (T)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + XA = atan2(r21,r22)
        rfYAngle = VmUtil<T>::atan2(mat.m21,mat.m22);
        rfZAngle = (M_PI*0.5);
        rfXAngle = (T)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
/** 
 * 
 * 
 * @param mat 
 * @param rfZAngle 
 * @param rfXAngle 
 * @param rfYAngle 
 * 
 * @return 
 */
template <typename T>
bool ToEulerAnglesZXY (const Matrix3<T>& mat,
		       T& rfZAngle, T& rfXAngle, T& rfYAngle) 
{
    // rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
    //        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
    //       -cx*sy           sx              cx*cy

    if ( mat.m21 < (T)1.0 )
    {
        if ( mat.m21 > -(T)1.0 )
        {
            rfZAngle = VmUtil<T>::atan2(-mat.m01,mat.m11);
            rfXAngle = VmUtil<T>::asin(mat.m21);
            rfYAngle = VmUtil<T>::atan2(-mat.m20,mat.m22);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - YA = -atan(r02,r00)
            rfZAngle = -VmUtil<T>::atan2(mat.m02,mat.m00);
            rfXAngle = -(M_PI*0.5);
            rfYAngle = (T)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + YA = atan2(r02,r00)
        rfZAngle = VmUtil<T>::atan2(mat.m02,mat.m00);
        rfXAngle = VmUtil<T>::PI * 0.5;
        rfYAngle = (T)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
/** 
 * 
 * 
 * @param mat 
 * @param rfZAngle 
 * @param rfYAngle 
 * @param rfXAngle 
 * 
 * @return 
 */
template <typename T>
bool ToEulerAnglesZYX (const Matrix3<T>& mat,
		       T& rfZAngle, T& rfYAngle, T& rfXAngle) 
{
    // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
    //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
    //       -sy              cy*sx           cx*cy

    if ( mat.m20 < (T)1.0 )
    {
        if ( mat.m20 > -(T)1.0 )
        {
            rfZAngle = VmUtil<T>::atan2(mat.m10,mat.m00);
            rfYAngle = VmUtil<T>::asin(-mat.m20);
            rfXAngle = VmUtil<T>::atan2(mat.m21,mat.m22);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
            rfZAngle = -VmUtil<T>::atan2(mat.m01,mat.m02);
            rfYAngle = (M_PI*0.5);
            rfXAngle = (T)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
        rfZAngle = VmUtil<T>::atan2(-mat.m01,-mat.m02);
        rfYAngle = -(M_PI*0.5);
        rfXAngle = (T)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
/** 
 * 
 * 
 * @param fYAngle 
 * @param fPAngle 
 * @param fRAngle 
 * @param mat 
 */
template <typename T>
void FromEulerAnglesXYZ (T fYAngle, T fPAngle, T fRAngle, 
			 Matrix3<T> *mat)
{
    T fCos, fSin;

    fCos = VmUtil<T>::cos(fYAngle);
    fSin = VmUtil<T>::sin(fYAngle);
    Matrix3<T> kXMat(
        (T)1.0,(T)0.0,(T)0.0,
        (T)0.0,fCos,-fSin,
        (T)0.0,fSin,fCos);

    fCos = VmUtil<T>::cos(fPAngle);
    fSin = VmUtil<T>::sin(fPAngle);
    Matrix3<T> kYMat(
        fCos,(T)0.0,fSin,
        (T)0.0,(T)1.0,(T)0.0,
        -fSin,(T)0.0,fCos);

    fCos = VmUtil<T>::cos(fRAngle);
    fSin = VmUtil<T>::sin(fRAngle);
    Matrix3<T> kZMat(
        fCos,-fSin,(T)0.0,
        fSin,fCos,(T)0.0,
        (T)0.0,(T)0.0,(T)1.0);

    *mat = kXMat*(kYMat*kZMat);
}
//----------------------------------------------------------------------------

/** 
 * 
 * 
 * @param fYAngle 
 * @param fPAngle 
 * @param fRAngle 
 * @param mat 
 */
template <typename T>
void FromEulerAnglesXZY (T fYAngle, T fPAngle, T fRAngle,
			 Matrix3<T> *mat)
{
    T fCos, fSin;

    fCos = VmUtil<T>::cos(fYAngle);
    fSin = VmUtil<T>::sin(fYAngle);
    Matrix3<T> kXMat(
        (T)1.0,(T)0.0,(T)0.0,
        (T)0.0,fCos,-fSin,
        (T)0.0,fSin,fCos);

    fCos = VmUtil<T>::cos(fPAngle);
    fSin = VmUtil<T>::sin(fPAngle);
    Matrix3<T> kZMat(
        fCos,-fSin,(T)0.0,
        fSin,fCos,(T)0.0,
        (T)0.0,(T)0.0,(T)1.0);

    fCos = VmUtil<T>::cos(fRAngle);
    fSin = VmUtil<T>::sin(fRAngle);
    Matrix3<T> kYMat(
        fCos,(T)0.0,fSin,
        (T)0.0,(T)1.0,(T)0.0,
        -fSin,(T)0.0,fCos);

    *mat = kXMat*(kZMat*kYMat);
}
//----------------------------------------------------------------------------
/** 
 * 
 * 
 * @param fYAngle 
 * @param fPAngle 
 * @param fRAngle 
 * @param mat 
 */
template <typename T>
void FromEulerAnglesYXZ (T fYAngle, T fPAngle, T fRAngle, 
			 Matrix3<T> *mat)
{
    T fCos, fSin;

    fCos = VmUtil<T>::cos(fYAngle);
    fSin = VmUtil<T>::sin(fYAngle);
    Matrix3<T> kYMat(
        fCos,(T)0.0,fSin,
        (T)0.0,(T)1.0,(T)0.0,
        -fSin,(T)0.0,fCos);

    fCos = VmUtil<T>::cos(fPAngle);
    fSin = VmUtil<T>::sin(fPAngle);
    Matrix3<T> kXMat(
        (T)1.0,(T)0.0,(T)0.0,
        (T)0.0,fCos,-fSin,
        (T)0.0,fSin,fCos);

    fCos = VmUtil<T>::cos(fRAngle);
    fSin = VmUtil<T>::sin(fRAngle);
    Matrix3<T> kZMat(
        fCos,-fSin,(T)0.0,
        fSin,fCos,(T)0.0,
        (T)0.0,(T)0.0,(T)1.0);

    *mat = kYMat*(kXMat*kZMat);
}
//----------------------------------------------------------------------------
/** 
 * 
 * 
 * @param fYAngle 
 * @param fPAngle 
 * @param fRAngle 
 * @param mat 
 */
template <typename T>
void FromEulerAnglesYZX (T fYAngle, T fPAngle, T fRAngle, 
			 Matrix3<T> *mat)
{
    T fCos, fSin;

    fCos = VmUtil<T>::cos(fYAngle);
    fSin = VmUtil<T>::sin(fYAngle);
    Matrix3<T> kYMat(
        fCos,(T)0.0,fSin,
        (T)0.0,(T)1.0,(T)0.0,
        -fSin,(T)0.0,fCos);

    fCos = VmUtil<T>::cos(fPAngle);
    fSin = VmUtil<T>::sin(fPAngle);
    Matrix3<T> kZMat(
        fCos,-fSin,(T)0.0,
        fSin,fCos,(T)0.0,
        (T)0.0,(T)0.0,(T)1.0);

    fCos = VmUtil<T>::cos(fRAngle);
    fSin = VmUtil<T>::sin(fRAngle);
    Matrix3<T> kXMat(
        (T)1.0,(T)0.0,(T)0.0,
        (T)0.0,fCos,-fSin,
        (T)0.0,fSin,fCos);

    *mat = kYMat*(kZMat*kXMat);
}
//----------------------------------------------------------------------------

/** 
 * 
 * 
 * @param fYAngle 
 * @param fPAngle 
 * @param fRAngle 
 * @param mat 
 */
template <typename T>
void FromEulerAnglesZXY (T fYAngle, T fPAngle, T fRAngle,
			 Matrix3<T> *mat)
{
    T fCos, fSin;

    fCos = VmUtil<T>::cos(fYAngle);
    fSin = VmUtil<T>::sin(fYAngle);
    Matrix3<T> kZMat(
        fCos,-fSin,(T)0.0,
        fSin,fCos,(T)0.0,
        (T)0.0,(T)0.0,(T)1.0);

    fCos = VmUtil<T>::cos(fPAngle);
    fSin = VmUtil<T>::sin(fPAngle);
    Matrix3<T> kXMat(
        (T)1.0,(T)0.0,(T)0.0,
        (T)0.0,fCos,-fSin,
        (T)0.0,fSin,fCos);

    fCos = VmUtil<T>::cos(fRAngle);
    fSin = VmUtil<T>::sin(fRAngle);
    Matrix3<T> kYMat(
        fCos,(T)0.0,fSin,
        (T)0.0,(T)1.0,(T)0.0,
        -fSin,(T)0.0,fCos);

    *mat = kZMat*(kXMat*kYMat);
}
//----------------------------------------------------------------------------

/** 
 * 
 * 
 * @param fYAngle 
 * @param fPAngle 
 * @param fRAngle 
 * @param mat 
 */
template <typename T>
void FromEulerAnglesZYX (T fYAngle, T fPAngle, T fRAngle, 
			 Matrix3<T> *mat)
{
    T fCos, fSin;

    fCos = VmUtil<T>::cos(fYAngle);
    fSin = VmUtil<T>::sin(fYAngle);
    Matrix3<T> kZMat(
        fCos,-fSin,(T)0.0,
        fSin,fCos,(T)0.0,
        (T)0.0,(T)0.0,(T)1.0);

    fCos = VmUtil<T>::cos(fPAngle);
    fSin = VmUtil<T>::sin(fPAngle);
    Matrix3<T> kYMat(
        fCos,(T)0.0,fSin,
        (T)0.0,(T)1.0,(T)0.0,
        -fSin,(T)0.0,fCos);

    fCos = VmUtil<T>::cos(fRAngle);
    fSin = VmUtil<T>::sin(fRAngle);
    Matrix3<T> kXMat(
        (T)1.0,(T)0.0,(T)0.0,
        (T)0.0,fCos,-fSin,
        (T)0.0,fSin,fCos);

    *mat = kZMat*(kYMat*kXMat);
}

VM_END_NS

#endif 
/*
 * Local Variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */

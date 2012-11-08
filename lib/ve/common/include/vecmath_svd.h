/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
/**
 * @file   vecmath_svd.h
 * @author Shohei Nobuhara
 * @date   Tue Sep  4 15:29:42 2007
 * 
 * @brief  vecmath の Matrix3 / Matrix4 で SVD を行う関数．
 *         同名のメンバ関数があるが，本格的 (?) なのが欲しかったので．
 *
 */
#ifndef VECMATH_SVD_H
#define VECMATH_SVD_H

#include <vecmath/vm_conf.h>
#include <vecmath/Matrix3.h>
#include <vecmath/Vector3.h>
#include <vecmath/Matrix4.h>
#include <vecmath/Vector4.h>

VM_BEGIN_NS

/**
 * u 行列，v 行列，特異値ベクトル w を，w の要素が降順になるように並び替える．
 *
 * @param u [in/out] U 行列（左側）
 * @param w [in/out] 特異値ベクトル（中央）
 * @param v [in/out] V 行列（右側）
 */
template <typename T, int M, int N>
static inline void svd_sort(T u[M][N], T w[N], T v[N][N]) {
  T t;
  for(int i=0; i<N; i++) {
    for(int j=i+1; j<N; j++) {
      if(w[i] < w[j]) {
        //swap eigenvalues
        t=w[j];
        w[j]=w[i];
        w[i]=t;
        //swap eigenvectors
        for(int k=0 ; k<N ; k++) {
          t=v[k][j];
          v[k][j]=v[k][i];
          v[k][i]=t;
        }
        for(int k=0 ; k<M ; k++) {
          t=u[k][j];
          u[k][j]=u[k][i];
          u[k][i]=t;
        }
      }
    }
  }
}


/**
 * MxNの行列を特異値分解する（M>=N でなくてはならない）．
 *
 * @param mat [in/out] MxN の配列．呼び出し時は入力行列を入れておくこと．終了時に左側行列 u として上書きされる．
 * @param w [out] 要素 N の配列．特異値が格納される．
 * @param v [out] NxN の配列．右側行列 t が格納される．
 */
template <typename T, int M, int N>
static inline void svd(T mat[M][N], T w[N], T v[N][N]) {
#define SVD_IMPL_SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define SVD_IMPL_MAX(a,b) (a > b ? a : b)
#define SVD_IMPL_MIN(a,b) (a > b ? b : a)
#define SVD_IMPL_PYTHAG(a,b) (sqrt(a*a + b*b))

  const int m = M, n = N;
  int flag,i,its,j,jj,k,l,nm;
  double anorm,c,f,g,h,s,scale,x,y,z;

  double rv1[n];// = std::vector<double>(n);
  g=scale=anorm=0.0;
  for (i=1;i<=n;i++) {
    l=i+1;
    rv1[i-1]=scale*g;
    g=s=scale=0.0;
    if (i <= m) {
      for (k=i;k<=m;k++) scale += fabs(mat[k-1][i-1]);
      if (scale) {
        for (k=i;k<=m;k++) {
          mat[k-1][i-1] /= scale;
          s += mat[k-1][i-1]*mat[k-1][i-1];
        }
        f=mat[i-1][i-1];
        g = -SVD_IMPL_SIGN(sqrt(s),f);
        h=f*g-s;
        mat[i-1][i-1]=f-g;
        for (j=l;j<=n;j++) {
          for (s=0.0,k=i;k<=m;k++) s += mat[k-1][i-1]*mat[k-1][j-1];
          f=s/h;
          for (k=i;k<=m;k++) mat[k-1][j-1] += f*mat[k-1][i-1];
        }
        for (k=i;k<=m;k++) mat[k-1][i-1] *= scale;
      }
    }
    w[i-1]=scale *g;
    g=s=scale=0.0;
    if (i <= m && i != n) {
      for (k=l;k<=n;k++) scale += fabs(mat[i-1][k-1]);
      if (scale) {
        for (k=l;k<=n;k++) {
          mat[i-1][k-1] /= scale;
          s += mat[i-1][k-1]*mat[i-1][k-1];
        }
        f=mat[i-1][l-1];
        g = -SVD_IMPL_SIGN(sqrt(s),f);
        h=f*g-s;
        mat[i-1][l-1]=f-g;
        for (k=l;k<=n;k++) rv1[k-1]=mat[i-1][k-1]/h;
        for (j=l;j<=m;j++) {
          for (s=0.0,k=l;k<=n;k++) s += mat[j-1][k-1]*mat[i-1][k-1];
          for (k=l;k<=n;k++) mat[j-1][k-1] += s*rv1[k-1];
        }
        for (k=l;k<=n;k++) mat[i-1][k-1] *= scale;
      }
    }
    anorm=SVD_IMPL_MAX(anorm,(fabs(w[i-1])+fabs(rv1[i-1])));
  }
  for (i=n;i>=1;i--) {
    if (i < n) {
      if (g) {
        for (j=l;j<=n;j++)
          v[j-1][i-1]=(mat[i-1][j-1]/mat[i-1][l-1])/g;
        for (j=l;j<=n;j++) {
          for (s=0.0,k=l;k<=n;k++) s += mat[i-1][k-1]*v[k-1][j-1];
          for (k=l;k<=n;k++) v[k-1][j-1] += s*v[k-1][i-1];
        }
      }
      for (j=l;j<=n;j++) v[i-1][j-1]=v[j-1][i-1]=0.0;
    }
    v[i-1][i-1]=1.0;
    g=rv1[i-1];
    l=i;
  }
  for (i=SVD_IMPL_MIN(m,n);i>=1;i--) {
    l=i+1;
    g=w[i-1];
    for (j=l;j<=n;j++) mat[i-1][j-1]=0.0;
    if (g) {
      g=1.0/g;
      for (j=l;j<=n;j++) {
        for (s=0.0,k=l;k<=m;k++) s += mat[k-1][i-1]*mat[k-1][j-1];
        f=(s/mat[i-1][i-1])*g;
        for (k=i;k<=m;k++) mat[k-1][j-1] += f*mat[k-1][i-1];
      }
      for (j=i;j<=m;j++) mat[j-1][i-1] *= g;
    } else for (j=i;j<=m;j++) mat[j-1][i-1]=0.0;
    ++mat[i-1][i-1];
  }
  for (k=n;k>=1;k--) {
    for (its=1;its<=30;its++) {
      flag=1;
      for (l=k;l>=1;l--) {
        nm=l-1;
        if ((double)(fabs(rv1[l-1])+anorm) == anorm) {
          flag=0;
          break;
        }
        if ((double)(fabs(w[nm-1])+anorm) == anorm) break;
      }
      if (flag) {
        c=0.0;
        s=1.0;
        for (i=l;i<=k;i++) {
          f=s*rv1[i-1];
          rv1[i-1]=c*rv1[i-1];
          if ((double)(fabs(f)+anorm) == anorm) break;
          g=w[i-1];
          h=SVD_IMPL_PYTHAG(f,g);
          w[i-1]=h;
          h=1.0/h;
          c=g*h;
          s = -f*h;
          for (j=1;j<=m;j++) {
            y=mat[j-1][nm-1];
            z=mat[j-1][i-1];
            mat[j-1][nm-1]=y*c+z*s;
            mat[j-1][i-1]=z*c-y*s;
          }
        }
      }
      z=w[k-1];
      if (l == k) {
        if (z < 0.0) {
          w[k-1] = -z;
          for (j=1;j<=n;j++) v[j-1][k-1] = -v[j-1][k-1];
        }
        break;
      }
      //if (its == 30) nrerror("no convergence in 30 svdcmp iterations");
      x=w[l-1];
      nm=k-1;
      y=w[nm-1];
      g=rv1[nm-1];
      h=rv1[k-1];
      f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
      g=SVD_IMPL_PYTHAG(f,1.0);
      f=((x-z)*(x+z)+h*((y/(f+SVD_IMPL_SIGN(g,f)))-h))/x;
      c=s=1.0;
      for (j=l;j<=nm;j++) {
        i=j+1;
        g=rv1[i-1];
        y=w[i-1];
        h=s*g;
        g=c*g;
        z=SVD_IMPL_PYTHAG(f,h);
        rv1[j-1]=z;
        c=f/z;
        s=h/z;
        f=x*c+g*s;
        g = g*c-x*s;
        h=y*s;
        y *= c;
        for (jj=1;jj<=n;jj++) {
          x=v[jj-1][j-1];
          z=v[jj-1][i-1];
          v[jj-1][j-1]=x*c+z*s;
          v[jj-1][i-1]=z*c-x*s;
        }
        z=SVD_IMPL_PYTHAG(f,h);
        w[j-1]=z;
        if (z) {
          z=1.0/z;
          c=f*z;
          s=h*z;
        }
        f=c*g+s*y;
        x=c*y-s*g;
        for (jj=1;jj<=m;jj++) {
          y=mat[jj-1][j-1];
          z=mat[jj-1][i-1];
          mat[jj-1][j-1]=y*c+z*s;
          mat[jj-1][i-1]=z*c-y*s;
        }
      }
      rv1[l-1]=0.0;
      rv1[k-1]=f;
      w[k-1]=x;
    }
  }

#undef SVD_IMPL_SIGN
#undef SVD_IMPL_MAX
#undef SVD_IMPL_MIN
#undef SVD_IMPL_PYTHAG
}

/**
 * SVD, decompose m into u * w * v'
 *
 * @param m [in] 3x3 matrix.
 * @param u [out] 3x3 matrix.
 * @param w [out] 3x3 diagonal matrix.
 * @param v [out] 3x3 matrix, not v'.
 */
template<typename T>
inline void svd(const Matrix3<T> & m,
                Matrix3<T> * u,
                Matrix3<T> * w,
                Matrix3<T> * v) {
  T _m[3][3] = {
    { m.m00, m.m01, m.m02 },
    { m.m10, m.m11, m.m12 },
    { m.m20, m.m21, m.m22 },
  };
  T _v[3][3];
  T _w[3];
  svd<T, 3, 3>(_m, _w, _v);
  svd_sort<T, 3, 3>(_m, _w, _v);
  if(u) {
    u->set(_m[0][0], _m[0][1], _m[0][2],
           _m[1][0], _m[1][1], _m[1][2],
           _m[2][0], _m[2][1], _m[2][2]);
  }
  if(w) {
    w->set(_w[0], 0, 0,
           0, _w[1], 0,
           0, 0, _w[2]);
  }
  if(v) {
    v->set(_v[0][0], _v[0][1], _v[0][2],
           _v[1][0], _v[1][1], _v[1][2],
           _v[2][0], _v[2][1], _v[2][2]);
  }
}

/**
 * SVD, decompose m into u * w * v'
 *
 * @param m [in] 3x3 matrix.
 * @param u [out] 3x3 matrix.
 * @param w [out] 3x1 vector.
 * @param v [out] 3x3 matrix, not v'.
 */
template<typename T>
inline void svd(const Matrix3<T> & m,
                Matrix3<T> * u,
                Vector3<T> * w,
                Matrix3<T> * v) {
  T _m[3][3] = {
    { m.m00, m.m01, m.m02 },
    { m.m10, m.m11, m.m12 },
    { m.m20, m.m21, m.m22 },
  };
  T _v[3][3];
  T _w[3];
  svd<T, 3, 3>(_m, _w, _v);
  svd_sort<T, 3, 3>(_m, _w, _v);
  if(u) {
    u->set(_m[0][0], _m[0][1], _m[0][2],
           _m[1][0], _m[1][1], _m[1][2],
           _m[2][0], _m[2][1], _m[2][2]);
  }
  if(w) {
    w->set(_w[0], _w[1], _w[2]);
  }
  if(v) {
    v->set(_v[0][0], _v[0][1], _v[0][2],
           _v[1][0], _v[1][1], _v[1][2],
           _v[2][0], _v[2][1], _v[2][2]);
  }
}

/**
 * SVD, decompose m into u * w * v'
 *
 * @param m [in] 4x4 matrix.
 * @param u [out] 4x4 matrix.
 * @param w [out] 4x1 vector.
 * @param v [out] 4x4 matrix, not v'.
 */
template<typename T>
inline void svd(const Matrix4<T> & m,
                Matrix4<T> * u,
                Matrix4<T> * w,
                Matrix4<T> * v) {
  T _m[4][4] = {
    { m.m00, m.m01, m.m02, m.m03 },
    { m.m10, m.m11, m.m12, m.m13 },
    { m.m20, m.m21, m.m22, m.m23 },
    { m.m30, m.m31, m.m32, m.m33 },
  };
  T _v[4][4];
  T _w[4];
  svd<T, 4, 4>(_m, _w, _v);
  svd_sort<T, 4, 4>(_m, _w, _v);
  if(u) {
    u->set(_m[0][0], _m[0][1], _m[0][2], _m[0][3],
           _m[1][0], _m[1][1], _m[1][2], _m[1][3],
           _m[2][0], _m[2][1], _m[2][2], _m[2][3],
           _m[3][0], _m[3][1], _m[3][2], _m[3][3]);
  }
  if(w) {
    w->set(_w[0], 0, 0, 0,
           0, _w[1], 0, 0,
           0, 0, _w[2], 0,
           0, 0, 0, _w[3]);
  }
  if(v) {
    v->set(_v[0][0], _v[0][1], _v[0][2], _v[0][3],
           _v[1][0], _v[1][1], _v[1][2], _v[1][3],
           _v[2][0], _v[2][1], _v[2][2], _v[2][3],
           _v[3][0], _v[3][1], _v[3][2], _v[3][3]);
  }
}


/**
 * SVD, decompose m into u * w * v'
 *
 * @param m [in] 4x4 matrix.
 * @param u [out] 4x4 matrix.
 * @param w [out] 4x1 vector.
 * @param v [out] 4x4 matrix, not v'.
 */
template<typename T>
inline void svd(const Matrix4<T> & m,
                Matrix4<T> * u,
                Vector4<T> * w,
                Matrix4<T> * v) {
  T _m[4][4] = {
    { m.m00, m.m01, m.m02, m.m03 },
    { m.m10, m.m11, m.m12, m.m13 },
    { m.m20, m.m21, m.m22, m.m23 },
    { m.m30, m.m31, m.m32, m.m33 },
  };
  T _v[4][4];
  T _w[4];
  svd<T, 4, 4>(_m, _w, _v);
  svd_sort<T, 4, 4>(_m, _w, _v);
  if(u) {
    u->set(_m[0][0], _m[0][1], _m[0][2], _m[0][3],
           _m[1][0], _m[1][1], _m[1][2], _m[1][3],
           _m[2][0], _m[2][1], _m[2][2], _m[2][3],
           _m[3][0], _m[3][1], _m[3][2], _m[3][3]);
  }
  if(w) {
    w->set(_w[0], _w[1], _w[2], _w[3]);
  }
  if(v) {
    v->set(_v[0][0], _v[0][1], _v[0][2], _v[0][3],
           _v[1][0], _v[1][1], _v[1][2], _v[1][3],
           _v[2][0], _v[2][1], _v[2][2], _v[2][3],
           _v[3][0], _v[3][1], _v[3][2], _v[3][3]);
  }
}

extern "C" int dgesvd_(char *jobu, char *jobvt, int *m, int *n, double *a,
                       int *lda, double *s, double *u, int * ldu, double *vt,
                       int *ldvt, double *work, int *lwork, int *info);

int svd_3x3(const double * a_9, double * u_9, double * d_3, double * vt_9) {
  char jobu = 'A';
  char jobvt = 'A';
  int m = 3;
  int n = 3;
  int lda = 3;
  int ldu = 3;
  int ldvt = 3;

  int info;

#if 0
  // query the size of workspace.
  // MKL requires much more workspace than MAX(3*MIN(m,n)+MAX(m,n), 5*MIN(m,n)) !!!
  static int lwork = -1;
  double required_size;
  dgesvd_(&jobu, &jobvt, &m, &n, a, &lda, m_d, m_u, &ldu, m_vt, &ldvt, &required_size, &lwork, &info);
  lwork = static_cast<int>(required_size);
  //fprintf(stderr, "lwork >= %d", lwork);

  //lwork = MAX(3*MIN(m,n)+MAX(m,n), 5*MIN(m,n));

  double * work = new double[lwork];
  assert(work);
#endif

  int lwork = 64;
  double work[64];

  dgesvd_(&jobu, &jobvt, &m, &n, const_cast<double *>(a_9), &lda, d_3, u_9, &ldu, vt_9, &ldvt, work, &lwork, &info);

  //delete[] work;

  return info;
}

int svd_3x3(const LA::matrix3_t & a_, LA::matrix3_t * u_, LA::matrix3_t * d_, LA::matrix3_t * v_) {
  double a[9] = { a_.m00, a_.m10, a_.m20,
                  a_.m01, a_.m11, a_.m21,
                  a_.m02, a_.m12, a_.m22 };
  double u[9], d[3], vt[9];
  int ret = svd_3x3(a, u, d, vt);
  u_->set(u[0], u[3], u[6],
          u[1], u[4], u[7],
          u[2], u[5], u[8]);
  d_->set(d[0], 0, 0,
          0, d[1], 0,
          0, 0, d[2]);
  v_->set(vt[0], vt[1], vt[2],
          vt[3], vt[4], vt[5],
          vt[6], vt[7], vt[8]);
  return ret;
}

VM_END_NS

#endif //VECMATH_SVD_H

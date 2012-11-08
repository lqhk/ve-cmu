/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef VECMATH_UTIL_H
#define VECMATH_UTIL_H

#include <string>
#include <ios>
#include <sstream>
#include <vector>

#include <vecmath/vm_conf.h>

VM_BEGIN_NS

template<typename T> inline std::string to_octave(const std::vector< Vector3<T> > & v) {
  std::ostringstream oss;
  oss.setf(std::ios::scientific, std::ios::floatfield);

  oss << "[ ";
  for(unsigned int i=0 ; i<v.size() ; i++) {
    oss << v[i].x << " " << v[i].y << " " << v[i].z << "; ";
  }
  oss << "]";

  return oss.str();
}

template<typename T> inline std::string to_octave(const Tuple4<T> & v) {
  std::ostringstream oss;
  oss.setf(std::ios::scientific, std::ios::floatfield);
  oss << "[ " << v.x << "; " << v.y << "; " << v.z << "; " << v.w << " ]";
  return oss.str();
}

template<typename T> inline std::string to_octave(const Tuple3<T> & v) {
  std::ostringstream oss;
  oss.setf(std::ios::scientific, std::ios::floatfield);
  oss << "[ " << v.x << "; " << v.y << "; " << v.z << " ]";
  return oss.str();
}


template<typename T> inline std::string to_octave(const Matrix4<T> & v) {
  std::ostringstream oss;
  oss.setf(std::ios::scientific, std::ios::floatfield);
  oss << "[ "
      << v.m00 << " " << v.m01 << " " << v.m02 << " " << v.m03 << ";\n"
      << v.m10 << " " << v.m11 << " " << v.m12 << " " << v.m13 << ";\n"
      << v.m20 << " " << v.m21 << " " << v.m22 << " " << v.m23 << ";\n"
      << v.m30 << " " << v.m31 << " " << v.m32 << " " << v.m33 << "]";
  return oss.str();
}


template<typename T> inline std::string to_octave(const Matrix3<T> & v) {
  std::ostringstream oss;
  oss.setf(std::ios::scientific, std::ios::floatfield);
  oss << "[ "
      << v.m00 << " " << v.m01 << " " << v.m02 << ";\n"
      << v.m10 << " " << v.m11 << " " << v.m12 << ";\n"
      << v.m20 << " " << v.m21 << " " << v.m22 << "]";
  return oss.str();
}

template<typename T> inline std::string to_oogl_point(const Vector3<T> & v, float r=0, float g=0, float b=0, float a=1) {
  std::ostringstream oss;
  oss.setf(std::ios::scientific, std::ios::floatfield);
  oss << "VECT\n1 1 1\n1\n1\n"
      << v.x << " " << v.y << " " << v.z << "\n"
      << r << " " << g << " " << b << " " << a << "\n";
  return oss.str();
}

template<typename T> inline std::string to_oogl_line(const Vector3<T> & v1, const Vector3<T> & v2, float r=0, float g=0, float b=0, float a=1) {
  std::ostringstream oss;
  oss.setf(std::ios::scientific, std::ios::floatfield);
  oss << "VECT\n1 2 1\n2\n1\n"
      << v1.x << " " << v1.y << " " << v1.z << "\n"
      << v2.x << " " << v2.y << " " << v2.z << "\n"
      << r << " " << g << " " << b << " " << a << "\n";
  return oss.str();
}

VM_END_NS



#endif //VECMATH_UTIL_H

/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef VM_H
#define VM_H

#include <vecmath/vecmath.h>
#include <string>
#include <cstring>
#include <cstdio>

namespace VM {

  typedef double real_t;

  typedef VM_VECMATH_NS::Vector2<real_t> vector2_t;

  typedef VM_VECMATH_NS::Matrix3<real_t> matrix3_t;

  typedef VM_VECMATH_NS::Vector3<real_t> vector3_t;

  typedef VM_VECMATH_NS::Matrix4<real_t> matrix4_t;

  typedef VM_VECMATH_NS::Vector4<real_t> vector4_t;

  typedef VM_VECMATH_NS::Quat4<real_t> quaternion_t;

  inline void to_array(const matrix3_t & m, real_t * x) {
    x[0] = m.m00; x[1] = m.m01; x[2] = m.m02;
    x[3] = m.m10; x[4] = m.m11; x[5] = m.m12;
    x[6] = m.m20; x[7] = m.m21; x[8] = m.m22;
  }

  inline void to_array(const vector3_t & m, real_t * x) {
    x[0] = m.x; x[1] = m.y; x[2] = m.z;
  }

  inline std::string to_octave(const matrix3_t & m) {
    char buf[1024];
    snprintf(buf, sizeof(buf),
             "[ [%.16e %.16e %.16e];\n"
             "  [%.16e %.16e %.16e];\n"
             "  [%.16e %.16e %.16e]; ]\n",
             m.m00, m.m01, m.m02,
             m.m10, m.m11, m.m12,
             m.m20, m.m21, m.m22);
    return buf;
  }
}

#endif //VM_H

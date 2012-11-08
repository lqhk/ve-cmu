/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef KDTREEXX_H
#define KDTREEXX_H

#include "kdtree++/kdtree.hpp"

namespace KDTree {

  template<typename T, int N>
  struct key_t {
    typedef T value_type;

    value_type value[N];
    const static int dim = N;

    const value_type & operator[](size_t i) const {
      return value[i];
    }

    value_type & operator[](size_t i) {
      return value[i];
    }

  };

  typedef key_t<double,4> key4d_t;
  typedef KDTree<key4d_t::dim, key4d_t> kdtree4d_t;

  typedef key_t<double,3> key3d_t;
  typedef KDTree<key3d_t::dim, key3d_t> kdtree3d_t;

  typedef key_t<double,2> key2d_t;
  typedef KDTree<key2d_t::dim, key2d_t> kdtree2d_t;
}

#endif

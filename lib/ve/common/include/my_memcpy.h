/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef MY_MEMCPY_H
#define MY_MEMCPY_H

// compile with "auto-vectorization" feature, e.g., -O3 or -ftree-vectorize
// much faster than memcpy in libc6 with x64
template<typename T>
inline void * my_memcpy(void * __restrict b, const void * __restrict a, size_t n) {
  T * s1 = (T *)b;
  const T * s2 = (const T*)a;
  n /= sizeof(T);
  for(; 0<n; --n)*s1++ = *s2++;
  return b;
}

#endif

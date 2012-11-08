/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#include <cstdio>
#include "common1.h"
#include "common2.h"

int main() {
  int x = sizeof_int();
  int y = sizeof_void();
  printf("sizeof int - sizeof void * = %d\n", x - y);

  return 0;
}


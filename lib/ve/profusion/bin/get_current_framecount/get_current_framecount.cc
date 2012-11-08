/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#include "libviewplus/PF_EZInterface.h"
#include "libpfcmu/capture++.h"
#include "libpfcmu/util.h"
#include "boost_opt_util.h"
#include "trace.h"
#include "my_memcpy.h"

int main(int argc, char * argv[]) {
  const int FPS = 100;
  const int N = 100;

  PFCMU::Capture capture;
  capture.init(0, FPS);
  capture.start(1);

  timestamp_t ts = 0;
  for(int i=0 ; i<N ; i++) {
    PF_EZImage * img = capture.grab();
    ts = img->timestamp;
  }
  capture.stop();

  fprintf(stdout, "%llu\n", ts);

  return 0;
}


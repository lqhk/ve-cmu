/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#include "pfcmu_config.h"

#include <iostream>
#include <vector>

#include <highgui.h>

#include "libpfcmu/util.h"
#include "rawfile.h"
#include "boost_opt_util.h"
#include "trace.h"

int main(int argc, char * argv[]) {
  boost::program_options::options_description cmdline("Command line options");
  cmdline.add_options()
    ("help,h", "show help message")
    ("src,s",
     boost::program_options::value<std::string>(),
     "[MANDATORY] Input filename (/disks/local/out.dat)")
    ("fps,f",
     boost::program_options::value<unsigned int>(),
     "[MANDATORY] FPS (25 or 100)")
    ("debug", "Debug mode")
    ;

  boost::program_options::variables_map parameter_map = boost_opt_check(cmdline, argc, argv);

  const std::string SRC_FNAME = boost_opt_string(parameter_map, "src");
  const unsigned int FPS = parameter_map["fps"].as<unsigned int>();
  const int WIDTH = (FPS == 100 ? 320 : 640);
  const int HEIGHT = (FPS == 100 ? 240 : 480);
  const int CAMS = PFCMU::CAMS;
  const int DEBUG_MODE = parameter_map.count("debug") ? 1 : 0;
  const timestamp_t SKIP = FPS == 100 ? 1 : 4;

  int error_count = 0;

  PFCMU::RAWFile rawfile;
  rawfile.open(SRC_FNAME.c_str(), WIDTH, HEIGHT);
  fprintf(stderr, "%s has %zd images\n", SRC_FNAME.c_str(), rawfile.size());

  IplImage * bayer = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);

  std::vector<timestamp_t> ts(CAMS);
  timestamp_t ts_prev = 0;
  for(PFCMU::RAWFile::const_iterator itr=rawfile.begin() ;
      itr != rawfile.end() ;
      itr ++) {
    for(int i=0 ; i<CAMS ; i++) {
      itr.extract(i, bayer);
      ts[i] = PFCMU::get_timestamp(bayer->imageData);
    }

    if( (itr.frame() - rawfile.begin().frame()) % 50 == 0 ) {
      fprintf(stderr, "\n");
      if(! DEBUG_MODE) {
        fprintf(stderr, "%012zd : ", itr.frame());
      }
    }
    if(DEBUG_MODE) {
      fprintf(stderr, "%08zd : %08llu = %08llu + %llu * %zd\n", itr.frame(), ts[0], ts[0] - itr.frame()*SKIP, SKIP, itr.frame());
      if(ts[0] - ts_prev != SKIP && itr.frame()>2) {
        fprintf(stderr, "%08zd : timestamp error!\n", itr.frame());
        error_count ++;
      }
    } else {
      if(ts[0] - ts_prev != SKIP && itr.frame()>2) {
        fprintf(stderr, "x");
      } else {
        fprintf(stderr, ".");
      }
    }

    for(int j=1 ; j<CAMS ; j++) {
      if(ts[j] != ts[0]) {
        fprintf(stderr, "%08zd : img[%02d] has different timestamp %llu\n", itr.frame(), j, ts[j]);
        error_count ++;
      }
    }

    ts_prev = ts[0];
  }

  fprintf(stdout, "%s has %d errors\n", SRC_FNAME.c_str(), error_count);

  return 0;
}


/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
/**
 * @file   server.cc
 * @author Shohei NOBUHARA <nob@i.kyoto-u.ac.jp>
 * @date   Sun Feb 13 21:28:53 2011
 * 
 * @brief  Live streaming server implemented as a blocking/synchronized TCP server.
 * 
 */

#include <boost/asio.hpp>

#include "pfcmu_config.h"
#include "libviewplus/PF_EZInterface.h"
#include "libpfcmu/linux_aio.h"
#include "libpfcmu/capture++.h"
#include "libpfcmu/util.h"
#include "boost_opt_util.h"
#include "trace.h"
#include "my_memcpy.h"

int main(int argc, char * argv[]) {
  boost::program_options::options_description cmdline("Command line options");
  cmdline.add_options()
    ("help,h", "show help message")
    ("fps,f",
     boost::program_options::value<unsigned int>(),
     "[MANDATORY] FPS (25 or 100)")
    ("camera,c",
     boost::program_options::value<unsigned int>()->default_value(0),
     "Device ID (0, 1, ...)")
    ("port,p",
     boost::program_options::value<unsigned int>()->default_value(10000),
     "TCP port")
    ("shutter",
     boost::program_options::value<double>()->default_value(10),
     "Shutter speed (ms)")
    ("gain",
     boost::program_options::value<double>()->default_value(10),
     "Gain (dB)")
    ("c_ringnum",
     boost::program_options::value<unsigned int>()->default_value(1),
     "Ringbuf size for cam -> mem")
    ("verbose,v",
     boost::program_options::value<unsigned int>()->default_value(0),
     "verbosity")
    ("max_priority", "Set highest priority (only root can do this)")
    ("debug", "Debugging mode")
    ;

  boost::program_options::variables_map parameter_map = boost_opt_check(cmdline, argc, argv);

  const unsigned int FPS = parameter_map["fps"].as<unsigned int>();
  const unsigned int PORT = parameter_map["port"].as<unsigned int>();
  const unsigned int CAMERA = parameter_map["camera"].as<unsigned int>();
  const unsigned int C_RINGNUM = parameter_map["c_ringnum"].as<unsigned int>();
  const int ENABLE_MAX_PRIORITY = parameter_map.count("max_priority") ? 1 : 0;
  const unsigned int FRAME_INC = FPS == 100 ? 1 : 4;
  const double CAM_SHUTTER = parameter_map["shutter"].as<double>();
  const double CAM_GAIN = parameter_map["gain"].as<double>();
  const unsigned int VERBOSE = parameter_map["verbose"].as<unsigned int>();
  const int DEBUG_MODE = parameter_map.count("debug") ? 1 : 0;

  SET_VERBOSITY(VERBOSE);

  if(ENABLE_MAX_PRIORITY) {
    PFCMU::set_max_priority();
  }

  TRACE(1, "Camera init\n");
  PFCMU::Capture capture;
  capture.init(CAMERA, FPS);

  const int MEMSIZE = capture.width() * capture.height();

  TRACE(1, "Capture start\n");
  capture.start(C_RINGNUM);

  // set params
  TRACE(1, "Camera set shutter = %f\n", CAM_SHUTTER);
  capture.set_shutter(CAM_SHUTTER);
  TRACE(1, "Camera set gain = %f\n", CAM_GAIN);
  capture.set_gain(CAM_GAIN);


  // kill old frames
  for(unsigned int i=0 ; i<C_RINGNUM+1 ; i++) {
    capture.grab();
  }

  // set up the server which accepts only ONE client (= sync server)

  try {
    using boost::asio::ip::tcp;
    boost::asio::io_service io_service;

    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), PORT));

    TRACE(1, "Server is ready at port %d\n", PORT);
std::cout << "Server is Ready" << std::endl;


    for (;;) {
      tcp::iostream stream;
      acceptor.accept(*stream.rdbuf());
      // send the first msg
std::string str = capture.to_string();
      stream << std::string("100 ") + capture.to_string() + "\n";
//std::cout << std::string("100 ") + capture.to_string() << std::endl;	//hao
std::cout << std::string("100 ") + str << std::endl;

      // receive inputs
      for(std::string line ; std::getline(stream, line) ; ) {
        if(DEBUG_MODE) {
          fprintf(stderr, "line len=%zd :", line.length());
          for(unsigned int i=0 ; i<line.length() ; i++) {
            fprintf(stderr, " %02x", (unsigned char)(line[i]));

          }
          fprintf(stderr, "\n");
        }


        // skip if empty
        if(line.empty() || line[0] == '\r') {
          continue;
        }


        // parse the command
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if(cmd == "BYE") {
          TRACE(1, "BYE");
          break;
        } else if(cmd == "PGM") {
          int camid = 0;
          iss >> camid;
          if(camid < 0 || camid >= 24) {
            camid = 0;
          }
          TRACE(1, "PGM CAMID=%d\n", camid);

          stream << "P5\n"
                 << capture.width() << "\n" << capture.height() << "\n"
                 << "255\n"
                 << std::flush;
std::cout << "P5\n"<<capture.width() << "\n" << capture.height() << "\n" << "255\n"<<std::flush;
          PF_EZImage * img = capture.grab();
          PFCMU::embed_timestamp(img);
          TRACE(1, "Sending CAM%02d, TS=%llu\n", camid, PFCMU::get_timestamp(img->imageArray[camid]));
          stream.write(reinterpret_cast<const char *>(img->imageArray[camid]), MEMSIZE);
std::cout << "sended" <<std::endl;
        } else {
          TRACE(1, "unknown command = '%s'\n", line.c_str());
        }
      }
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  capture.stop();

  return 0;


#if 0
  timestamp_t ts_prev = 0;

  for(int i=0 ; i<N ; i++) {
    PF_EZImage * img = capture.grab();
    const timestamp_t ts_curr = img->timestamp;
    if(ts_curr < START) {
      i=-1;
      fprintf(stderr, "timestamp = %llu < %llu, skip\n", ts_curr, START);
      continue;
    }

    PFCMU::embed_timestamp(img);

#if 1
    for(int j=0 ; j<PFCMU::CAMS ; j++) {
      ASSERT(ts_curr == PFCMU::get_timestamp(img->imageArray[j]),
             "%d[%d]: %llu != %llu\n", i, j, ts_curr, PFCMU::get_timestamp(img->imageArray[j]));
    }
#endif
    
    if(ts_curr - ts_prev != FRAME_INC && i>(int)C_RINGNUM) {
      fprintf(stderr, "%08d : %08llu (=%08llu+%u*%08d), drop=%llu\n",
              i, ts_curr, ts_curr-i*FRAME_INC, FRAME_INC, i,
              (ts_curr - ts_prev)/FRAME_INC-1);
    } else {
      fprintf(stderr, "%08d : %08llu (=%08llu+%u*%08d), OK\n",
              i, ts_curr, ts_curr-i*FRAME_INC, FRAME_INC, i);
    }
    ts_prev = ts_curr;

  }
#endif

  capture.stop();

  return 0;
}


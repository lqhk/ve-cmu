/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
/**
 * @file   capture.cc
 * @author Shohei NOBUHARA <nob@i.kyoto-u.ac.jp>
 * @date   Tue Jan  4 19:35:08 2011
 * 
 * @brief  Capture RAW video stream from a ProFusion-CMU
 *
 * Before exec this program, make sure that
 * -# ProFusion-CMU  detected by system (use lspci | grep 1a63:1066),
 * -# 'vp1066' kernel module is loaded (use lsmod | grep VP1066), and
 * -# '/dev/vpcpro0' exists as a char device.
 *
 */

#include "libviewplus/PF_EZInterface.h"
#include "libpfcmu/linux_aio.h"
#include "libpfcmu/capture++.h"
#include "libpfcmu/util.h"
#include "libpfcmu/mmapped_file.h"
#include "boost_opt_util.h"
#include "trace.h"
#include "my_memcpy.h"
#include "pfcmu_config.h"
#include "stringf.h"

static void dump_info(const std::string & capture_to_json,
		      const int curr,
		      const int total,
		      const timestamp_t ts,
		      const int framedrop,
		      PFCMU::MMappedFile * mfile) {
  const int sz = mfile->size();
  memset(mfile->buf(), '\n', sz);
  int ret = snprintf((char*)(mfile->buf()),
		     sz, 
		     "{\n%s\n"
		     "\t\"curr\": %d,\n"
		     "\t\"total\": %d,\n"
		     "\t\"framecount\": %llu,\n"
		     "\t\"framedrop\": %d,\n"
		     "}\n", 
		     capture_to_json.c_str(),
		     curr,
		     total,
		     ts,
		     framedrop);
  mfile->buf()[ret] = '\n';
  //ASSERT(ret >= sz, "JSON output has been truncated.\n");
  mfile->sync();
}

int main(int argc, char * argv[]) {

  boost::program_options::options_description cmdline("Command line options");
  cmdline.add_options()
    ("help,h", "show help message")
    ("num,n",
     boost::program_options::value<int>(),
     "[MANDATORY] N of frames to capture")
    ("fps,f",
     boost::program_options::value<unsigned int>(),
     "[MANDATORY] FPS (25 or 100)")
    ("out,o",
     boost::program_options::value<std::string>(),
     "Output filename (/disks/local/out.dat)")
    ("live,l",
     boost::program_options::value<std::string>(),
     "Live output dir (/live). Ramdisk (/dev/ram15, for example) is STRONGLY recommended. Export this directory by NFS and mount it remotely to get the live view of the camera. Note: you can use tmpfs as a memory-based filesystem, but tmpfs cannot be exported by NFS since NFS works on top of a block device.")
    ("start,s",
     boost::program_options::value<unsigned int>()->default_value(0),
     "The frame to start capture (use 0 to start immediately)")
    ("camera,c",
     boost::program_options::value<unsigned int>()->default_value(0),
     "Device ID (0, 1, ...)")
    ("shutter",
     boost::program_options::value<double>()->default_value(31),
     "Shutter speed (ms)")
    ("gain",
     boost::program_options::value<double>()->default_value(5.5),
     "Gain (dB)")
    ("c_ringnum",
     boost::program_options::value<unsigned int>()->default_value(8),
     "Ringbuf size for cam -> mem")
    ("d_ringnum",
     boost::program_options::value<unsigned int>()->default_value(8),
     "Ringbuf size for mem -> disk")
    ("d_align",
     boost::program_options::value<unsigned int>()->default_value(4096),
     "Alignment size for disk AIO")
    ("skip",
     boost::program_options::value<unsigned int>()->default_value(400),
     "Skip frames before capture. This will help avoiding frame drops by shifting the HDD in a steady state.")
    ("debug", "Debug mode")
    ;

  boost::program_options::variables_map parameter_map = boost_opt_check(cmdline, argc, argv);

  fprintf(stderr, "Command line options:\n%s\n", boost_opt_to_string(cmdline, parameter_map, "  ").c_str());

  const std::string OUT_FNAME = boost_opt_string(parameter_map, "out");
  const int N = parameter_map["num"].as<int>();
  const unsigned int FPS = parameter_map["fps"].as<unsigned int>();
  const timestamp_t START = parameter_map["start"].as<unsigned int>();
  const unsigned int CAMERA = parameter_map["camera"].as<unsigned int>();
  const unsigned int C_RINGNUM = parameter_map["c_ringnum"].as<unsigned int>();
  const unsigned int D_RINGNUM = parameter_map["d_ringnum"].as<unsigned int>();
  const unsigned int D_ALIGN = parameter_map["d_align"].as<unsigned int>();
  const unsigned int SKIP = parameter_map["skip"].as<unsigned int>();
  const int DEBUG_MODE = parameter_map.count("debug") ? 1 : 0;
  const unsigned int FRAME_INC = FPS == 100 ? 1 : 4;
  const double CAM_SHUTTER = parameter_map["shutter"].as<double>();
  const double CAM_GAIN = parameter_map["gain"].as<double>();
  const std::string LIVE_DIR=boost_opt_string(parameter_map, "live");

  TRACE(1, "Max priority\n");
  PFCMU::set_max_priority();

  TRACE(1, "Camera: init %d fps\n", FPS);
  PFCMU::Capture capture;
  capture.init(CAMERA, FPS);


  PFCMU::libaio::writer_t writer;
  if(! OUT_FNAME.empty()) {
    TRACE(1, "Output: init\n");
    writer.init(OUT_FNAME.c_str(), capture.memsize(), D_RINGNUM, N, D_ALIGN);
  } else {
    TRACE(1, "Output: no output (dry run)\n");
  }

  PFCMU::MMappedFile mfile[26];
  const int LIVE_P6HEADER_SIZE = 15;
  const int LIVE_WIDTHSTEP_DS = capture.width() / 2 * 3;
  const int LIVE_WIDTHSTEP_THUMB = capture.width() * 3;
  const int LIVE_CAMIMG_BEGIN = 0;
  const int LIVE_CAMIMG_END = 24;
  const int LIVE_THUMB = 24;
  const int LIVE_INFO = 25;
  const int LIVE_INFO_BYTES = 4096;

  if(! LIVE_DIR.empty()) {
    TRACE(1, "Live: init\n");
    char buf[LIVE_P6HEADER_SIZE + 1];
    int len = snprintf(buf, LIVE_P6HEADER_SIZE+1, "P6\n%d %d\n255\n", capture.width()/2, capture.height()/2);
    ASSERT(len == LIVE_P6HEADER_SIZE, "len=%d, LIVE_P6HEADER_SIZE=%d\n", len, LIVE_P6HEADER_SIZE);
    ASSERT(buf[LIVE_P6HEADER_SIZE-1] == 0x0A);

    for(int i=LIVE_CAMIMG_BEGIN ; i<LIVE_CAMIMG_END ; i++) {
      mfile[i].open((LIVE_DIR + Tools::stringf("/%02d.ppm", i+1)).c_str(), capture.memsize_single_ds() + LIVE_P6HEADER_SIZE);
      memcpy(mfile[i].buf(), buf, LIVE_P6HEADER_SIZE);
    }

    len = snprintf(buf, LIVE_P6HEADER_SIZE+1, "P6\n%d %d\n255\n", capture.width(), capture.height());
    ASSERT(len == LIVE_P6HEADER_SIZE, "len=%d, LIVE_P6HEADER_SIZE=%d\n", len, LIVE_P6HEADER_SIZE);
    ASSERT(buf[LIVE_P6HEADER_SIZE-1] == 0x0A);

    mfile[LIVE_THUMB].open((LIVE_DIR + "/thumb.ppm").c_str(), capture.memsize_single()*3 + LIVE_P6HEADER_SIZE);
    memcpy(mfile[LIVE_THUMB].buf(), buf, LIVE_P6HEADER_SIZE);

    mfile[LIVE_INFO].open((LIVE_DIR + "/info.json").c_str(), LIVE_INFO_BYTES, '\n');
  } else {
    TRACE(1, "Live: no output (dry run)\n");
  }

  TRACE(1, "Camera: start transmission\n");
  int max_sg_len = capture.start(C_RINGNUM);

  // set params (should be done AFTER capture.start())
  TRACE(1, "Camera: set shutter = %f\n", CAM_SHUTTER);
  capture.set_shutter(CAM_SHUTTER);
  TRACE(1, "Camera: set gain = %f\n", CAM_GAIN);
  capture.set_gain(CAM_GAIN);

  fprintf(stderr, "%s\n", capture.to_string().c_str());
  std::string json_str = capture.to_json();

  timestamp_t ts_prev = 0;

  // kill old frames
  TRACE(1, "Camera: killing %d frames\n", C_RINGNUM + SKIP);
  for(unsigned int i=0 ; i<C_RINGNUM+SKIP ; i++) {
    capture.grab();
    const timestamp_t ts = capture.get_framecount();

    if(writer.is_initialized()) {
      int id = writer.get_available_slot_id();
      capture.copy_all(writer.buf(id));
      writer.write(id, i);
    }
    if(mfile[0].is_initialized()) {
      for(int j=LIVE_CAMIMG_BEGIN ; j<LIVE_CAMIMG_END ; j++) {
	capture.copy_ds(mfile[j].buf() + LIVE_P6HEADER_SIZE, j, LIVE_WIDTHSTEP_DS);
	mfile[j].sync();
      }
      capture.copy_thumb(mfile[LIVE_THUMB].buf() + LIVE_P6HEADER_SIZE, LIVE_WIDTHSTEP_THUMB);
      mfile[LIVE_THUMB].sync();

      dump_info(json_str, 0, N, ts, 0, &(mfile[LIVE_INFO]));
    }
    ts_prev = ts;
  }

  int error_count  = 0;

  for(int i=0 ; i<N ; i++) {
    // retrieve the next frame
    capture.grab();
    // get the framecount
    const timestamp_t ts_curr = capture.get_framecount();
    // the framecount is younger than START?
    if(ts_curr < START) {
      // skip this frame
      i=-1;
      fprintf(stderr, "timestamp = %llu < %llu, skip\n", ts_curr, START);
      ts_prev = ts_curr;
      continue;
    }

    // embed the framecount into each images
    capture.embed_framecount();

    // real capture?
    if(writer.is_initialized()) {
      // get a slot to write
      int id = writer.get_available_slot_id();
      // fprintf(stderr, "Slot %d\n", id);
      // copy all the data into write buffer
      capture.copy_all(writer.buf(id));
      // tell the kerenl to write this slot
      writer.write(id, i);
    }

    if(mfile[0].is_initialized()) {
      // single images
      for(int j=LIVE_CAMIMG_BEGIN ; j<LIVE_CAMIMG_END ; j++) {
	capture.copy_ds(mfile[j].buf() + LIVE_P6HEADER_SIZE, j, LIVE_WIDTHSTEP_DS);
	mfile[j].sync();
      }

      // thumbnail
      capture.copy_thumb(mfile[LIVE_THUMB].buf() + LIVE_P6HEADER_SIZE, LIVE_WIDTHSTEP_THUMB);
      mfile[LIVE_THUMB].sync();

      // JSON
      dump_info(json_str, i+1, N, ts_curr, error_count, &(mfile[LIVE_INFO]));
    }
    
#if 0
    for(int j=0 ; j<PFCMU::CAMS ; j++) {
      ASSERT(ts_curr == capture.get_embedded_framecount(j),
             "%d[%d]: %lld != %zd\n", i, j, ts_curr, capture.get_embedded_framecount(j));
    }
#endif
    
    // check the framecount consistency and give eye-candy outputs
    if(! DEBUG_MODE && i%1000==0) {
      // start new line at every 1000 frames
      fprintf(stderr, "\n%012d : ", i);
    }
    //if(ts_curr - ts_prev != FRAME_INC && i>(int)C_RINGNUM) {
    if(ts_curr - ts_prev != FRAME_INC) {
      // frame dropped!
      if(DEBUG_MODE) {
        fprintf(stderr, "\n%08d : %08llu (=%08llu+%u*%08d), drop=%llu\n",
                i, ts_curr, ts_curr-i*FRAME_INC, FRAME_INC, i,
                (ts_curr - ts_prev)/FRAME_INC-1);
      } else {
        fprintf(stderr, "X");
      }
      error_count++;
    } else {
      // no error
      if(DEBUG_MODE) {
        fprintf(stderr, "%08d : %08llu (=%08llu+%u*%08d), OK, total err=%d\n",
                i, ts_curr, ts_curr-i*FRAME_INC, FRAME_INC, i, error_count);
      } else {
        if(i%25==0) {
          fprintf(stderr, ".");
        }
      }
    }
    
    
    ts_prev = ts_curr;
  }
  
  fprintf(stderr, "\n\nCapture finished (%d errors, max sg=%d)\n", error_count, max_sg_len);
  
  capture.stop();
  
  return 0;
}


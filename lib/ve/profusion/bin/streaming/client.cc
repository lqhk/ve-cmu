/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
/**
 * @file   client.cc
 * @author Shohei NOBUHARA <nob@i.kyoto-u.ac.jp>
 * @date   Sun Feb 13 21:25:54 2011
 * 
 * @brief  Live streaming viewer
 * 
 */

#include <boost/asio.hpp>
#include <cv.h>
#include <highgui.h>

#include "libpfcmu/util.h"
#include "boost_opt_util.h"
#include "trace.h"
#include "pfcmu_config.h"

int main(int argc, char * argv[]) {
  boost::program_options::options_description cmdline("Command line options");
  cmdline.add_options()
    ("help,h", "show help message")
    ("server,s",
     boost::program_options::value<std::string>(),
     "[MANDATORY] Streaming server name")
    ("port,p",
     boost::program_options::value<std::string>()->default_value("10000"),
     "TCP port")
    ;

  boost::program_options::variables_map parameter_map = boost_opt_check(cmdline, argc, argv);

  const std::string SERVER_NAME = boost_opt_string(parameter_map, "server");
  const std::string PORT = parameter_map["port"].as<std::string>();

  CvFont font;
  cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 1.0, 1.0, 0);

  int camid = 0;
  cvNamedWindow("main");
  cvCreateTrackbar("CAMID", "main", &camid, 23, NULL);
  IplImage * buf1 = NULL;
  IplImage * buf3 = NULL;

  // set up the server which accepts only ONE client (= sync server)
  try {
    using boost::asio::ip::tcp;

    tcp::iostream stream(SERVER_NAME.c_str(), PORT);
    std::string header;
    std::getline(stream, header);
    //std::cout << header << std::endl;	//hao

    PFCMU::timestamp_t ts_prev=0;
    for(;;) {
      // send "PGM CAMID" command
      char text[1024];
      snprintf(text, sizeof(text), "PGM %d", camid);
      stream << text << std::endl;

      // receive PGM data
      int width, height, color;
      stream >> header >> width >> height >> color;
//std::cout << header << std::endl;	//hao
      stream.get(); // kill 1-byte ('newline') right after the color
      //assert(header == "P5");

      // setup the buffer
      if(NULL == buf1 || buf1->width != width || buf1->height != height) {
        cvReleaseImage(&buf1);
        cvReleaseImage(&buf3);
        buf1 = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
        buf3 = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
      }

      // receive the image
      stream.read(buf1->imageData, width*height);

      // convert to BGR
      cvCvtColor(buf1, buf3, PFCMU::CV_BAYER2BGR);

      // write the timestamp embedded in the image
      PFCMU::timestamp_t ts_curr = PFCMU::get_timestamp(buf1->imageData);
      snprintf(text, sizeof(text), "%010llu (%llu)", ts_curr, ts_curr - ts_prev);
      ts_prev = ts_curr;
      cvPutText(buf3, text, cvPoint(0,32), &font, CV_RGB(255,0,0));

      // show the image
      cvShowImage("main", buf3);
      int key = cvWaitKey(5);
      if('q' == (key & 0xff)) {
        stream << "BYE" << std::endl;
        break;
      }

    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}


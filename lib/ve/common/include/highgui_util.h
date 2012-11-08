/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef HIGHGUI_UTIL_H
#define HIGHGUI_UTIL_H

#include <string>
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>

#include "ringbuf.h"

namespace HighGUI {
  class ScaledWindow {
  public:
    ScaledWindow() : m_buf(NULL), m_log(3) {
    }

    ScaledWindow(const char * window_name, float scale) : m_buf(NULL), m_log(3) {
      init(window_name, scale);
    }

    void init(const char * window_name, float scale) {
      m_window_name = window_name;
      m_scale = scale;
      if(m_scale > 0) {
        cvNamedWindow(window_name, CV_WINDOW_AUTOSIZE);
      }

      const float font_scale = 2.0;
      m_font_size = (int)(32 * font_scale);
      cvInitFont(&m_font, CV_FONT_HERSHEY_SIMPLEX, font_scale/2.0, font_scale);

      m_log_life = 0;
    }

    void show(const IplImage * img) {
      if(m_scale <= 0) return;

      const CvSize src_sz = cvGetSize(img);
      const CvSize dst_sz = { (int)(src_sz.width * m_scale), (int)(src_sz.height * m_scale) };
      if(m_buf == NULL) {
        m_buf = cvCreateImage(dst_sz, img->depth, img->nChannels);
        assert(m_buf);
      } else if(m_buf->width != dst_sz.width ||
                m_buf->height != dst_sz.height ||
                m_buf->depth != img->depth ||
                m_buf->nChannels != img->nChannels) {
        cvReleaseImage(&m_buf);
        m_buf = cvCreateImage(dst_sz, img->depth, img->nChannels);
      }
      assert(m_buf);

      cvResize(img, m_buf);

      if(m_log_life > 0) {
        int row = 1;
        for(unsigned int i=0 ; i<m_log.size() ; i++) {
          if(m_log[i].empty()) continue;
          draw_text(m_buf, m_log[i].c_str(), 1, row++, CV_RGB(255,0,0));
        }
        m_log_life --;
      }

      cvShowImage(m_window_name.c_str(), m_buf);
    }

    void draw_text(IplImage * buf, const char * msg, float row, float col, CvScalar color) const {
      std::string str(msg);
      std::string::size_type curr = 0;
      std::string::size_type prev = 0;
      do {
        curr = str.find("\n", prev);
        std::string sub = curr == std::string::npos ? str.substr(prev,str.length()-prev) : str.substr(prev, curr-prev);
        //TRACE(1, "line : %s, prev=%d, curr=%d\n", sub.c_str(), prev, curr);

        draw_textline(buf, sub.c_str(), row, col, color);
        col ++;

        prev = curr+1;
      } while (curr != std::string::npos);
    }

    void log(const char * msg) {
      m_log.push_back(msg);
      m_log_life = 10;
    }

  private:
    void draw_textline(IplImage * buf, const char * msg, float row, float col, CvScalar color) const {
      cvPutText(buf, msg, cvPoint(row*m_font_size,col*m_font_size), &m_font, color);
    }

    std::string m_window_name;
    IplImage * m_buf;

    // font
    float m_scale;
    CvFont m_font;
    int m_font_size;

    // log
    ringbuf<std::string> m_log;
    int m_log_life;
  };


  template<int event_down, int event_up, int flag_button>
  struct DragManager {
    CvPoint ps;
    CvPoint pe;

    enum {
      INACTIVE = 0,
      DRAGGING = 1,
      DRAGGED = 2,
    };
    int stat;

    void draw(IplImage * img) const {
      const static CvScalar color[] = {
        CV_RGB(255,0,0),
        CV_RGB(255,255,0),
        CV_RGB(0,255,0)
      };
      if(stat == INACTIVE) return;

      cvRectangle(img, ps, pe, color[stat]);
    }

    DragManager() { reset(); }

    void reset() {
      ps = cvPoint(-1,-1);
      pe = cvPoint(-1,-1);
      stat = INACTIVE;
    }

    virtual void on_drag_begin(int event, int x, int y, int flags) {
      fprintf(stderr, "Begin  %d, %d, %d, %d\n", event, x, y, flags);
    }

    virtual void on_dragging(int event, int x, int y, int flags) {
    }

    virtual void on_drag_end(int event, int x, int y, int flags) {
      fprintf(stderr, "End    %d, %d, %d, %d\n", event, x, y, flags);
    }

    virtual void on_drag_cancel(int event, int x, int y, int flags) {
      fprintf(stderr, "Cancel %d, %d, %d, %d\n", event, x, y, flags);
    }

    void on_mouse(int event, int x, int y, int flags) {
      // flags から numlock などを外す
      flags = flags & (CV_EVENT_FLAG_LBUTTON|CV_EVENT_FLAG_RBUTTON|CV_EVENT_FLAG_MBUTTON);

      switch(event) {
      case event_down:
        ps.x = x;
        ps.y = y;
        stat = INACTIVE; // not DRAGGING
        on_drag_begin(event, x, y, flags);
        break;
      case event_up:
        if(stat == DRAGGING && flags == flag_button) {
          // 他のボタンと同時に押されているときは，キャンセルとする
          pe.x = x;
          pe.y = y;
          stat = DRAGGED;
          on_drag_end(event, x, y, flags);
        } else {
          on_drag_cancel(event, x, y, flags);
          reset();
          return;
        }
        break;
      }

      if((flags&flag_button) == flag_button && stat != DRAGGED) {
        pe.x = x;
        pe.y = y;
        stat = DRAGGING;
        on_dragging(event, x, y, flags);
      }

    }
  };

  typedef DragManager<CV_EVENT_RBUTTONDOWN, CV_EVENT_RBUTTONUP, CV_EVENT_FLAG_RBUTTON> DragManagerR;
  typedef DragManager<CV_EVENT_LBUTTONDOWN, CV_EVENT_LBUTTONUP, CV_EVENT_FLAG_LBUTTON> DragManagerL;
  typedef DragManager<CV_EVENT_MBUTTONDOWN, CV_EVENT_MBUTTONUP, CV_EVENT_FLAG_MBUTTON> DragManagerM;
}


#endif //HIGHGUI_UTIL_H

/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef PFCMU_RAWFILE_H
#define PFCMU_RAWFILE_H

#include <cstdio>
#include <cstdlib>
#include <inttypes.h>
#include <sys/stat.h>
#include <opencv/cxcore.h>

#include "trace.h"
#include "pfcmu_config.h"

namespace PFCMU {
  class RAWFile {
  public:
    class const_iterator {
      FILE * m_fp;
      off64_t m_frame;
      int m_width;
      int m_height;
    public:
      const_iterator(FILE *, off64_t, int, int);

      void extract(int camid, IplImage * bayer_img) const;
      off64_t frame() const {
        return m_frame;
      }

      const_iterator & operator += (const int &i);
      const_iterator & operator -= (const int &i);
      const_iterator & operator ++ () {
        (*this) += 1;
        return (*this);
      }
      const_iterator operator ++ (int) {
        (*this) += 1;
        return (*this);
      }
      bool operator == (const const_iterator & i) const;
      bool operator != (const const_iterator & i) const;
      bool operator < (const const_iterator & i) const;
    };

    RAWFile();
    ~RAWFile();

    void open(const char * filename, int width, int height);

    const_iterator begin() const {
      return at(0);
    }

    const_iterator end() const {
      return at(size());
    }

    const_iterator at(off64_t index) const {
      return const_iterator(m_fp, index, m_width, m_height);
    }

    size_t size() const {
      return m_size;
    }

    static size_t framecount(const char * filename, size_t blocksize) {
      struct stat64 buf;
      int ret = stat64(filename, &buf);
      if(ret) {
        DIE(1, "cannot open %s by stat64", filename);
      }
      return buf.st_size / blocksize;
    }

  private:
    FILE * m_fp;
    size_t m_size;
    int m_width;
    int m_height;
  };
}

inline PFCMU::RAWFile::RAWFile() : m_fp(NULL) {
}

inline PFCMU::RAWFile::~RAWFile() {
  if(m_fp) {
    fclose(m_fp);
    m_fp = NULL;
  }
}

inline void PFCMU::RAWFile::open(const char * filename, int width, int height) {
  if(m_fp) {
    fclose(m_fp);
    m_fp = NULL;
  }

  m_fp = fopen64(filename, "r");

  if(!m_fp) {
    DIE(1, "cannot open %s\n", filename);
  }

  m_size = framecount(filename, width*height*PFCMU::CAMS);
  m_width = width;
  m_height = height;
}

inline PFCMU::RAWFile::const_iterator::const_iterator(FILE * fp, off64_t frame, int w, int h) : m_fp(fp), m_frame(frame), m_width(w), m_height(h) {
}

inline void PFCMU::RAWFile::const_iterator::extract(int i, IplImage * img) const {
  off64_t ret = fseeko64(m_fp, m_frame*m_width*m_height*PFCMU::CAMS+m_width*m_height*i, SEEK_SET);
  if(ret) {
    DIE(1, "cannot seek to %zd[%d]\n", m_frame, i);
  }

  ASSERT(img->widthStep == m_width);
  ASSERT(img->height == m_height);
  ASSERT(img->nChannels == 1);
  ASSERT(img->depth == IPL_DEPTH_8U);

  size_t blocks = fread(img->imageData, m_width*m_height, 1, m_fp);
  if( blocks != 1 ) {
    DIE(1, "cannot read at %zd[%d]\n", m_frame, i);
  }
}

inline PFCMU::RAWFile::const_iterator & PFCMU::RAWFile::const_iterator::operator+=(const int &i) {
  m_frame += i;
  return *this;
}

inline PFCMU::RAWFile::const_iterator & PFCMU::RAWFile::const_iterator::operator -= (const int &i) {
  m_frame -= i;
  return *this;
}

inline bool PFCMU::RAWFile::const_iterator::operator == (const const_iterator & i) const {
  return m_frame == i.m_frame;
}

inline bool PFCMU::RAWFile::const_iterator::operator != (const const_iterator & i) const {
  return m_frame != i.m_frame;
}

inline bool PFCMU::RAWFile::const_iterator::operator < (const const_iterator & i) const {
  return m_frame < i.m_frame;
}


#endif

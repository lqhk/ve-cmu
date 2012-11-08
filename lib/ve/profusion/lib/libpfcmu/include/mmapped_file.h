/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
/**
 * @file   mmapped_file.h
 * @author Shohei NOBUHARA <nob@i.kyoto-u.ac.jp>
 * @date   Thu Feb 17 19:36:27 2011
 * 
 * @brief  Wrapper class for Linux AIO
 * 
 */
#ifndef PFCMU_MMAPPED_FILE_H
#define PFCMU_MMAPPED_FILE_H

#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <linux/falloc.h>
#include "my_memcpy.h"

namespace PFCMU {
  class MMappedFile {
  public:
    MMappedFile() : m_fd(-1), m_map(NULL) {}

    ~MMappedFile() {
      close();
    }
    
    void open(const char * filename,
	      size_t size,
	      int val=0) {
      m_fd = ::open(filename, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
      // pre-allocate the file
      if( 0 != posix_fallocate64(m_fd, 0, size) ) {
	perror("posix_fallocate64");
      }
      // fill it out
      for(size_t i=0 ; i<size ; i++) {
	int ret = ::write(m_fd, &val, 1);
	if(ret != 1) {
	  perror("write");
	  abort();
	}
      }
      lseek(m_fd, 0, SEEK_SET);
      // m-map
      m_map = mmap(NULL, size, PROT_WRITE, MAP_SHARED, m_fd, 0);
      if( MAP_FAILED == m_map ) {
	perror("mmap");
	abort();
      }
      m_size = size;
    }

    void close() {
      if(m_map) {
	if( 0 != munmap(m_map, m_size) ) {
	  perror("munmap");
	  abort();
	}
	m_map = NULL;
      }
      if(m_fd >= 0) {
	if( 0 != ::close(m_fd) ) {
	  perror("close");
	  abort();
	}
	m_fd = -1;
      }
    }

    void sync() {
      msync(m_map, m_size, MS_SYNC);
    }
    
    unsigned char * buf() {
      return (unsigned char *)m_map;
    }

    int is_initialized() const {
      return m_fd >= 0;
    }

    int size() const {
      return m_size;
    }

  private:
    int m_fd;
    void * m_map;
    int m_size;
  };
}

#endif

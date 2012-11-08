/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
/**
 * @file   linux_aio.h
 * @author Shohei NOBUHARA <nob@i.kyoto-u.ac.jp>
 * @date   Thu Feb 17 19:36:27 2011
 * 
 * @brief  Wrapper class for Linux AIO
 * 
 */
#ifndef PFCMU_AIO_H
#define PFCMU_AIO_H

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libaio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <linux/falloc.h>

// linux_aio is 10% faster than posix_aio, with kernel 2.6.32 + libc6 2.11

namespace PFCMU {
  namespace libaio {
    typedef intptr_t slot_id_t;
    typedef unsigned char byte_t;

    /**
     * Linux AIO wrapper
     */
    class writer_t {
    private:
      int fd;
      io_context_t ctx;
      int n_slots;

      struct iocb * obj;
      byte_t ** buf_aligned;
      size_t buf_size;
      int buf_count;

    public:
      writer_t() : fd(-1), n_slots(0), obj(NULL), buf_aligned(NULL) {
      }

      ~writer_t() {
        clean();
      }

      /**
       * Finish writing (waits until all current writing ends)
       */
      void clean() {
        if(fd != -1) {
          fsync(fd);
          io_destroy(ctx);
          close(fd);
          fd = -1;
        }

        if(obj) {
          free(obj);
          obj = NULL;
        }

        if(buf_aligned) {
          for(int i=0 ; i<n_slots ; i++) {
            if(buf_aligned[i]) {
              free(buf_aligned[i]);
            }
          }
          free(buf_aligned);
        }

        n_slots = 0;
      }

      /**
       * Test if the instance has been initialized (init() has been called).
       * 
       * @return 0 if not yet, positive values when initialized.
       */
      int is_initialized() const {
        return n_slots;
      }

      /**
       * Initialize and make the instance be ready to write
       * 
       * @param filename [in] output filename
       * @param blocksize [in] bytes in a single write
       * @param bufnum [in] ring buffer depth
       * @param count [in] number of blocks to be written (can be 0, or you can write more than this count. But the perfomance will drop significantly)
       * @param align [in] memory alignment for O_DIRECT (do not modify unless you know what you are doing)
       */
      void init(const char * filename, off64_t blocksize, off64_t bufnum, off64_t count, off64_t align=4096) {
        clean();

        this->n_slots = bufnum;
        this->buf_size = blocksize;
        // try with O_DIRECT first
        this->fd = open64(filename, O_CREAT|O_WRONLY|O_LARGEFILE|O_TRUNC|O_DIRECT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        if(this->fd < 0) {
          // try without O_DIRECT
          this->fd = open64(filename, O_CREAT|O_WRONLY|O_LARGEFILE|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
          if(this->fd < 0) {
            perror("open64");
	    fprintf(stderr, "Cannot open %s for writing. Please check the path and permission.\n", filename);
            abort();
          } else {
            fprintf(stderr, "WARNING: O_DIRECT is not available for %s.\n", filename);
          }
        }

        // pre-allocate the file space. this is very important for perfomance
        if(count) {
          if( 0 != posix_fallocate64(fd, 0, blocksize*count) ) {
            perror("posix_fallocate64");
	    fprintf(stderr, "Cannot preallocate %zd bytes for %s. Please check the available size of the disk (man df(1)).\n", blocksize * count, filename);
            abort();
          }
          //fallocate(fd, 0, 0, blocksize*count);
          if( 0 != posix_fadvise(fd, 0, blocksize*count, POSIX_FADV_SEQUENTIAL)) {
            perror("posix_fadv_sequential");
            abort();
          }
          if( 0 != posix_fadvise(fd, 0, blocksize*count, POSIX_FADV_NOREUSE)) {
            perror("posix_fadv_noreuse");
            abort();
          }
        }

        // init the io_context_t
        memset(&ctx, 0, sizeof(io_context_t));
        if( 0 != io_setup(n_slots, &ctx) ) {
          perror("io_setup");
          abort();
        }

        obj = (struct iocb *)malloc(sizeof(struct iocb) * n_slots);
        if(obj == NULL) {
          fprintf(stderr, "posix_memalign returned error\n"); // posix_memalign does not set errno.
        }

        buf_aligned = (byte_t **)malloc(sizeof(byte_t *) * n_slots);
        if(NULL == buf_aligned) {
          perror("malloc");
        }

        for(int i=0 ; i<n_slots ; i++) {
          void * p = NULL;
          if(0 != posix_memalign(&p, align, blocksize)) {
            fprintf(stderr, "posix_memalign returned error\n"); // posix_memalign does not set errno.
          }
          buf_aligned[i] = reinterpret_cast<byte_t *>(p);
        }

        buf_count = 0;
      }

      /**
       * Get a slot willing to serve.
       *
       * This function blocks until at least one slot becomes available
       * (= finishes current writing).
       *
       * @return slot ID
       */
      slot_id_t get_available_slot_id() {
        if(buf_count < n_slots) {
          buf_count += 1;
          return buf_count-1;
        }

        struct io_event event;
        int r = io_getevents(ctx, 1, 1, &event, NULL);
        assert(r == 1);
        //fprintf(stderr, "event.obj = %tx, slots[0].obj=%tx\n", (intptr_t)event.obj, (intptr_t)(obj));
        slot_id_t id = ((intptr_t)(event.obj) - (intptr_t)(obj)) / sizeof(struct iocb);
        //slot_id_t id = (slot_id_t)(event.obj->key);
        //fprintf(stderr, "slot[%zd] is available\n", id);
        //fprintf(stderr, "event.obj = %tx, slots[%zd].obj=%tx\n", (intptr_t)event.obj, id, (intptr_t)(obj+id));
        assert(event.obj == &(obj[id]));
        assert(event.res == buf_size);
        assert(event.res2 == 0);

        return id;
      }

      /**
       * Obtain the pointer to the buffer
       *
       * This function does not block.
       *
       * @param id [in] slot ID given by get_available_slot_id().
       * 
       * @return pointer to the buf of get_available_slot_id().
       */
      byte_t * buf(slot_id_t id) {
        return buf_aligned[id];
      }

      /**
       * Queue the slot for writing
       *
       * This function does not block.
       *
       * @param id [in] slot ID given by get_available_slot_id().
       * @param index [in] block position in the output file
       * 
       * @return 0 on success, negative on error.
       */
      int write(slot_id_t id, int index) {
        struct iocb * cb[1] = { &(obj[id]) };
        io_prep_pwrite(cb[0], fd, buf(id), buf_size, index*buf_size);
        int r = io_submit(ctx, 1, cb);
        if( r == 1 ) {
          return 0;
        } else {
          return r;
        }
      }
    };
  }
}

#endif

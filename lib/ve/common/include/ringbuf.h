/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef RINGBUF_H
#define RINGBUF_H

#include <vector>

template<typename T>
class ringbuf {
public:
  typedef T value_t;

  ringbuf(int N) : m_N(N), m_curr(-1), m_data(N) {
  }

  ringbuf(int N, value_t init) : m_N(N), m_curr(-1), m_data(N, init) {
  }

  void clear(value_t v) {
    container_t(m_data.size(), v).swap(m_data);
  }

  value_t & operator[](int i) {
    return m_data[rawidx(i)];
  }

  const value_t & operator[](int i) const {
    return m_data[rawidx(i)];
  }

  value_t & raw(int i) {
    return m_data[i];
  }

  const value_t & raw(int i) const {
    return m_data[i];
  }

  int rawidx(int i) const {
    i += size() + 1;
    i += curr();
    i %= size();
    return i;
  }

  const int curr() const { return m_curr; }

  unsigned int size() const {
    return m_N;
  }

  void push_back(value_t v) {
    m_curr += 1;
    m_curr %= m_N;
    m_data[m_curr] = v;
  }

private:
  typedef std::vector<value_t> container_t;
  const int m_N;
  int m_curr;
  container_t m_data;
};


#endif //RINGBUF_H

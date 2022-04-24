#include "logstream.h"

using namespace tiny_muduo;

template <int SIZE>
void FixedBuffer<SIZE>::CookieStart() {}

template <int SIZE>
void FixedBuffer<SIZE>::CookieEnd() {}

template <int SIZE>
FixedBuffer<SIZE>::FixedBuffer() : cur_(buf_) {
  SetCookie(CookieStart); 
}

template <int SIZE>
FixedBuffer<SIZE>::~FixedBuffer() {
  SetCookie(CookieEnd);
}

template class FixedBuffer<kLargeSize>;
template class FixedBuffer<kSmallSize>;

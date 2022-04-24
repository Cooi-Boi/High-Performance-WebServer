#include "logstream.h"

using namespace tiny_muduo;

template class FixedBuffer<kSmallSize>;
template class FixedBuffer<kLargeSize>;

template <int SIZE>
inline void FixedBuffer<SIZE>::CookieStart() {}

template <int SIZE>
inline void FixedBuffer<SIZE>::CookieEnd() {}

template <int SIZE>
FixedBuffer<SIZE>::FixedBuffer() : cur_(buf_) {
  SetCookie(CookieStart); 
}

template <int SIZE>
FixedBuffer<SIZE>::~FixedBuffer() {
  SetCookie(CookieEnd);
}

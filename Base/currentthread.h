#ifndef TINY_MUDUO_CURRENTTHREAD_H_
#define TINY_MUDUO_CURRENTTHREAD_H_

#include <stdint.h>

namespace CurrentThread {

extern __thread int t_cachedTid;
extern __thread char t_formattedTid[32];
extern __thread int t_formattedTidLength;

void CacheTid();

inline int tid() {
  if (__builtin_expect(t_cachedTid == 0, 0)) {
    CacheTid();
  }
  return t_cachedTid;
}

inline const char* tid2string() { return t_formattedTid; } 
inline int tidstringlength() { return t_formattedTidLength; }

} // namespace CurrentThread

#endif

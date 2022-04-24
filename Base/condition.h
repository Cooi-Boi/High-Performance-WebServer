#ifndef TINY_MUDUO_CONDITION_H_
#define TINY_MUDUO_CONDITION_H_

#include <pthread.h>

#include "mutex.h"
#include "noncopyable.h"

namespace tiny_muduo {

class Condition : public NonCopyAble {
 public:
  explicit Condition(MutexLock& mutex) : mutex_(mutex) {
    pthread_cond_init(&cond_, nullptr);
  }

  ~Condition() {
    pthread_cond_destroy(&cond_);
  }

  bool Wait() {
    return pthread_cond_wait(&cond_, mutex_.mutex()) == 0;
  }

  bool WaitForFewSeconds(double seconds);

  bool Notify() {
    return pthread_cond_signal(&cond_);
  }

  bool NotifyAll() {
    return pthread_cond_broadcast(&cond_);
  }

 private:
  MutexLock& mutex_;
  pthread_cond_t cond_;
}; 

}
#endif

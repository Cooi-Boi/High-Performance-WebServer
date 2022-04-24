#ifndef TINY_MUDUO_THREAD_H_
#define TINY_MUDUO_THREAD_H_

#include <stdio.h>
#include <sys/prctl.h>
#include <pthread.h>

#include <functional>
#include <string>

#include "latch.h"
#include "noncopyable.h"

using std::string;

namespace tiny_muduo {

static int thread_id_ = 1;

class Thread : public NonCopyAble {
 public:
  typedef std::function<void ()> ThreadFunc;

  Thread(const ThreadFunc& func, const string& name = string());
  ~Thread(); 

  void StartThread();
  void Join() { ::pthread_join(pthread_id_, nullptr); }

 private:
  pthread_t pthread_id_;
  ThreadFunc func_;
  Latch latch_;
  string name_;
};       


struct ThreadData {

typedef tiny_muduo::Thread::ThreadFunc ThreadFunc;
 
ThreadFunc func_;
Latch* latch_;
string name_;

ThreadData(ThreadFunc& func, Latch* latch, const string& name)
    : func_(func),
      latch_(latch),
      name_(name) {
}

void RunOneFunc() {
  latch_->CountDown();
  latch_ = nullptr;  
  char buf[32] = {0};
  snprintf(buf, sizeof(buf), "%d", (thread_id_++));
  ::prctl(PR_SET_NAME, name_.size() == 0 ? ("WorkThread " + string(buf)).data() : name_.data());
  func_();
}

};

}
#endif

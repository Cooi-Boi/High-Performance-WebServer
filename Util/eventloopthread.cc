#include "eventloopthread.h"

#include <pthread.h>
#include <functional>

#include "mutex.h"
#include "condition.h"
#include "eventloop.h"

using namespace tiny_muduo;

EventLoopThread::EventLoopThread()
    : loop_(nullptr),
      thread_(std::bind(&EventLoopThread::StartFunc, this)),
      mutex_(),
      cond_(mutex_) {
}

EventLoopThread::~EventLoopThread() {}        

EventLoop* EventLoopThread::StartLoop() {
  thread_.StartThread();
  EventLoop* loop = nullptr;
  {   
    MutexLockGuard lock(mutex_);
    while (loop_ == nullptr) {
      cond_.Wait();
    }
    loop = loop_;
  }
  return loop;
}

void EventLoopThread::StartFunc() {
  EventLoop loop;

  {
    MutexLockGuard lock(mutex_);
    loop_ = &loop;
    cond_.Notify();
  }

  loop_->Loop();
  {
    MutexLockGuard lock(mutex_);
    loop_ = nullptr;
  }
}

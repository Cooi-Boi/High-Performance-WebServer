#ifndef TINY_MUDUO_EVENTLOOP_H_
#define TINY_MUDUO_EVENTLOOP_H_

#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <sys/eventfd.h>
#include <pthread.h>

#include <vector>
#include <functional>
#include <memory>
#include <utility>

#include "mutex.h"
#include "epoller.h"
#include "currentthread.h"
#include "timestamp.h"
#include "timerqueue.h"
#include "noncopyable.h"

namespace tiny_muduo {

class Epoller;
class Channel;

class EventLoop : public NonCopyAble {
 public:
  typedef std::vector<Channel*> Channels;
  typedef std::function<void()> BasicFunc; 
  typedef std::vector<BasicFunc> ToDoList;  
  
  EventLoop();
  ~EventLoop();

  void RunAt(Timestamp timestamp, BasicFunc&& cb) {
    timer_queue_->AddTimer(timestamp, std::move(cb), 0.0);
  }

  void RunAfter(double wait_time, BasicFunc&& cb) {
    Timestamp timestamp(Timestamp::AddTime(Timestamp::Now(), wait_time)); 
    timer_queue_->AddTimer(timestamp, std::move(cb), 0.0);
  }

  void RunEvery(double interval, BasicFunc&& cb) {
    Timestamp timestamp(Timestamp::AddTime(Timestamp::Now(), interval)); 
    timer_queue_->AddTimer(timestamp, std::move(cb), interval);
  }

  bool IsInThreadLoop() { return CurrentThread::tid() == tid_; }
  void Update(Channel* channel) { epoller_->Update(channel); }
  void Remove(Channel* channel) { epoller_->Remove(channel); }

  void Loop();
  void HandleRead();
  void QueueOneFunc(BasicFunc func);
  void RunOneFunc(BasicFunc func);
  void DoToDoList();

 private:
  bool running_;
  pid_t tid_; 
  std::unique_ptr<Epoller> epoller_;
  int wakeup_fd_;
  std::unique_ptr<Channel> wakeup_channel_;
  std::unique_ptr<TimerQueue> timer_queue_;
  bool calling_functors_;
  Channels active_channels_;
  ToDoList to_do_list_;

  MutexLock mutex_;
};
 
}  // namespace tiny_muduo
#endif

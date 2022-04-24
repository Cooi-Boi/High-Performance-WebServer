#ifndef TINY_MUDUO_TIMERQUEUE_H_
#define TINY_MUDUO_TIMERQUEUE_H_

#include <unistd.h>
#include <sys/timerfd.h>

#include <set>
#include <vector>
#include <memory>
#include <utility>
#include <functional>

#include "timer.h"
#include "timestamp.h"
#include "noncopyable.h"
#include "logging.h"

namespace tiny_muduo {

class EventLoop;
class Channel;

class TimerQueue : public NonCopyAble {
 public:
  typedef std::function<void()> BasicFunc;

  TimerQueue(EventLoop* loop);
  ~TimerQueue();

  void ReadTimerFd() {
    uint64_t read_byte;
    ssize_t readn = ::read(timerfd_, &read_byte, sizeof(read_byte));
    if (readn != sizeof(read_byte)) {
      LOG_ERROR	 << "TimerQueue::ReadTimerFd read_size < 0";
    }
  }

  void HandleRead() {
    ReadTimerFd();
    Timestamp expiration_time(Timestamp::Now());

    active_timers_.clear();    
    auto end = timers_.lower_bound(TimerPair(Timestamp::Now(), reinterpret_cast<Timer*>(UINTPTR_MAX)));
    active_timers_.insert(active_timers_.end() , timers_.begin(), end);
    timers_.erase(timers_.begin(), end);
    
    for (const auto& timerpair : active_timers_) {
      timerpair.second->Run();
    } 
    ResetTimers();
  }

  void ResetTimers() {
    for (auto& timerpair: active_timers_) {
      if ((timerpair.second)->repeat()) {
        auto timer = timerpair.second;
        timer->Restart(Timestamp::Now());
        Insert(timer);
      } else {
        delete timerpair.second;
      }
    } 

    if (!timers_.empty()) {
      ResetTimer(timers_.begin()->second);
    }
  }

  bool Insert(Timer* timer) {
    bool reset_instantly = false;
    if (timers_.empty() || timer->expiration() < timers_.begin()->first ) {
      reset_instantly = true;
    }

    timers_.emplace(std::move(TimerPair(timer->expiration(), timer)));
    return reset_instantly;
  }

  void AddTimerInLoop(Timer* timer) {
    bool reset_instantly = Insert(timer);
    if (reset_instantly) {
      ResetTimer(timer);
    }
  }

  void ResetTimer(Timer* timer);
  void AddTimer(Timestamp timestamp, BasicFunc&& cb, double interval);

 private:
  typedef std::pair<Timestamp, Timer*> TimerPair;
  typedef std::set<TimerPair> TimersSet; 
  typedef std::vector<TimerPair> ActiveTimers;

  EventLoop* loop_;
  int timerfd_;
  std::unique_ptr<Channel> channel_; 

  TimersSet timers_;
  ActiveTimers active_timers_;
};

} // namesapce tiny_muduo

#endif

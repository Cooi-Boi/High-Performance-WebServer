#include "timerqueue.h"

#include <assert.h>
#include <sys/timerfd.h>

#include <cstring>

#include "channel.h"

using namespace tiny_muduo;

TimerQueue::TimerQueue(EventLoop* loop)
    : loop_(loop),
      timerfd_(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)),
      channel_(new Channel(loop_, timerfd_)) {
  channel_->SetReadCallback(std::bind(&TimerQueue::HandleRead, this));
  channel_->EnableReading();
}

TimerQueue::~TimerQueue() {
  channel_->DisableAll();
  loop_->Remove(channel_.get());
  close(timerfd_);

  for (const auto& timerpair : timers_) {
    delete timerpair.second;
  } 
}

void TimerQueue::AddTimer(Timestamp timestamp, BasicFunc&& cb, double interval) {
  Timer* timer(new Timer(timestamp, std::move(cb), interval));
  loop_->RunOneFunc(std::bind(&TimerQueue::AddTimerInLoop, this, timer));
}

void TimerQueue::ResetTimer(Timer* timer) {
  struct itimerspec new_;
  struct itimerspec old_;
  memset(&new_, '\0', sizeof(new_));
  memset(&old_, '\0', sizeof(old_));

  int64_t micro_seconds_dif = timer->expiration().microseconds() - Timestamp::Now().microseconds();
  if (micro_seconds_dif < 100) {
    micro_seconds_dif = 100;
  }
 
  new_.it_value.tv_sec = static_cast<time_t>(
      micro_seconds_dif / kMicrosecond2Second);
  new_.it_value.tv_nsec = static_cast<long>(
      (micro_seconds_dif % kMicrosecond2Second) * 1000);
  int ret = ::timerfd_settime(timerfd_, 0, &new_, &old_);
  assert(ret != -1);
  (void) ret;
}

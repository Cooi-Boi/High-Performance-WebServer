#ifndef TINY_MUDUO_TIMER_H_
#define TINY_MUDUO_TIMER_H_

#include <functional>

#include "timestamp.h"
#include "noncopyable.h"

namespace tiny_muduo {

class Timer : public NonCopyAble {
 public:
  typedef std::function<void()> BasicFunc;

  Timer(Timestamp timestamp, BasicFunc&& cb, double interval);

  void Restart(Timestamp now) {
    expiration_ = Timestamp::AddTime(now, interval_);
  }

  void Run() const {
    callback_();
  }

  Timestamp expiration() const { return expiration_; }
  bool repeat() const { return repeat_; }

 private:
  Timestamp expiration_; 
  BasicFunc callback_;
  double interval_; 
  bool repeat_;
};

} // namespace tiny_muduo

#endif

#ifndef TINY_MUDUO_ASYNCLOGGING_H_
#define TINY_MUDUO_ASYNCLOGGING_H_

#include <vector>
#include <memory>

#include "mutex.h"
#include "condition.h"
#include "latch.h"
#include "thread.h"
#include "logging.h"
#include "logstream.h"
#include "noncopyable.h"
#include "logfile.h"

namespace tiny_muduo {

static const double kBufferWriteTimeout = 3.0;
static const int64_t kSingleFileMaximumSize = 1024 * 1024 * 1024;

class AsyncLogging : public NonCopyAble {
 public:
  typedef FixedBuffer<kLargeSize> Buffer;
  typedef std::unique_ptr<Buffer> BufferPtr;
  typedef std::vector<BufferPtr> BufferVector;
  typedef std::unique_ptr<LogFile> LogFilePtr;

  AsyncLogging(const char* filepath = nullptr) 
      : running_(false),
        filepath_(filepath),
        mutex_(),
        cond_(mutex_),
        latch_(1),
        thread_(std::bind(&AsyncLogging::ThreadFunc, this), "AsyncLogThread"),
        current_(new Buffer()),
        next_(new Buffer()) {
  }

  ~AsyncLogging() {
    if (running_) {
      Stop();
    } 
  }
  
  void Stop() {
    running_ = false;
    cond_.Notify();
    thread_.Join();
  }
 
  void StartAsyncLogging() {
    running_ = true;
    thread_.StartThread(); 
    latch_.Wait();
  }
  
  void Append(const char* data, int len);
  void Flush();
  void ThreadFunc();

 private:
  bool running_;
  const char* filepath_;
  MutexLock mutex_;
  Condition cond_;
  Latch latch_;
  Thread thread_;
  BufferPtr current_;
  BufferPtr next_; 
  BufferVector buffers_to_write_; 
};

} // namespace tiny_muduo

#endif

#ifndef TINY_MUDUO_LOGFILE_H_
#define TINY_MUDUO_LOGFILE_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include <string>

#include "timestamp.h"

namespace tiny_muduo {

static const time_t kFlushInterval = 3;

class LogFile {
 public:
  LogFile(const char* filepath);
  ~LogFile();

  void Write(const char* data, int len);
  void Flush() { fflush(fp_); }

  int64_t writtenbytes() const { return written_bytes_; }

 private:
  FILE* fp_; 
  int64_t written_bytes_;
  time_t lastwrite_;
  time_t lastflush_; 
};

}

#endif

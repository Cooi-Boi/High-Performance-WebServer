#ifndef TINY_MUDUO_TIMESTAMP_H_
#define TINY_MUDUO_TIMESTAMP_H_

#include <stdio.h>
#include <sys/time.h>

#include <string>

#include "noncopyable.h"

const int kMicrosecond2Second = 1000 * 1000;

class Timestamp {
 public:
  Timestamp() : micro_seconds_(0) {}
  explicit Timestamp(int64_t micro_seconds) : micro_seconds_(micro_seconds) {}

  bool operator<(const Timestamp& rhs) const {
    return micro_seconds_ < rhs.microseconds();
  }

  bool operator==(const Timestamp& rhs) const {
    return micro_seconds_ == rhs.microseconds();
  }

  std::string ToFormattedDefaultLogString() const {
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(micro_seconds_ / kMicrosecond2Second);
    struct tm tm_time;
    localtime_r(&seconds, &tm_time);
    snprintf(buf, sizeof(buf), "%4d%02d%02d_%02d%02d%02d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    return buf;
  }

  std::string ToFormattedString() const {
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(micro_seconds_ / kMicrosecond2Second);
    struct tm tm_time;
    localtime_r(&seconds, &tm_time);
    int micro_seconds = static_cast<int>(micro_seconds_ % kMicrosecond2Second);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, micro_seconds);
    return buf;
  }

  int64_t microseconds() const { return micro_seconds_; } 

  static Timestamp Now();
  static Timestamp AddTime(const Timestamp& timestamp, double add_seconds);

 private:
  int64_t micro_seconds_;
};

inline Timestamp Timestamp::Now() {
  struct timeval time;
  gettimeofday(&time,NULL);
  return Timestamp(time.tv_sec * kMicrosecond2Second + time.tv_usec); 
}

inline Timestamp Timestamp::AddTime(const Timestamp& timestamp, double add_seconds) {
  int64_t add_microseconds = static_cast<int64_t>(add_seconds) * kMicrosecond2Second;   
  return Timestamp(timestamp.microseconds() + add_microseconds);
}

#endif

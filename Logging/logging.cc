#include "logging.h"

#include <utility>

using namespace tiny_muduo;
using namespace CurrentThread;

namespace CurrentThread {

__thread time_t t_lastsecond;
__thread char t_time[64];
__thread const int t_formattedtimeLength = 18; 
__thread char t_errorbuf[512];

} // namespace CurrentThread

const char* ErrorToString(int err) {
  return strerror_r(err, t_errorbuf, sizeof(t_errorbuf));
}

static const int kLogLevelStringLength = 6;

static void DefaultOutput(const char* data, int len) {
  fwrite(data, len, sizeof(char), stdout); 
}

static void DefaultFlush() {
  fflush(stdout);  
}

Logger::OutputFunc g_Output = DefaultOutput;
Logger::FlushFunc g_Flush = DefaultFlush;
Logger::Level g_level = Logger::Level::INFO;

void SetOutputFunc(Logger::OutputFunc func) {
  g_Output = func;
}

void SetFlushFunc(Logger::FlushFunc func) {
  g_Flush = func;
}

Logger::Level LogLevel() { return g_level; }
void SetLogLevel(tiny_muduo::Logger::Level nowlevel) { g_level = nowlevel; }

Logger::Implement::Implement(SourceClass&& source, int line, Level level)  
    : fileinfo_(std::move(source)),
      line_(line),
      level_(level) {
  FormattedTime();
  CurrentThread::tid();
}  

Logger::Implement::~Implement() {
  Finish();
  const LogStream::Buffer& buffer = stream_.buffer();
  g_Output(buffer.data(), buffer.len());
}

const char* Logger::Implement::GetLogLevel() const {
  switch(level_) {
    case DEBUG:
      return "DEBUG ";
    case INFO:
      return "INFO  ";
    case WARN:
      return "WARN  ";
    case ERROR:
      return "ERROR ";
    case FATAL:
      return "FATAL ";
  }   
  return nullptr;
} 

void Logger::Implement::FormattedTime() {
  Timestamp now = Timestamp::Now();
  time_t seconds = static_cast<time_t>(now.microseconds() / kMicrosecond2Second);
  int microseconds = static_cast<int>(now.microseconds() % kMicrosecond2Second);

  if (t_lastsecond != seconds) {
    struct tm tm_time;
    localtime_r(&seconds, &tm_time);
    snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d.",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    t_lastsecond = seconds;
  }

  char buf[32] = {0};
  int microlen = snprintf(buf, sizeof(buf), "%06d ", microseconds);
  
  stream_ << GeneralTemplate(t_time, t_formattedtimeLength) << GeneralTemplate(buf, microlen);
  stream_ << GeneralTemplate(CurrentThread::t_formattedTid, CurrentThread::t_formattedTidLength);
  stream_ << GeneralTemplate(GetLogLevel(), kLogLevelStringLength);
}

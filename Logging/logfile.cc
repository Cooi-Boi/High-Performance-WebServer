#include "logfile.h"

using namespace tiny_muduo;

LogFile::LogFile(const char* filepath = nullptr)
    : fp_(::fopen(filepath, "ae")),
      written_bytes_(0),
      lastwrite_(0),
      lastflush_(0) {
  if (!fp_) {
    std::string DefaultPath = std::move("./LogFiles/LogFile_" +
                              Timestamp::Now().Timestamp::ToFormattedDefaultLogString() +
                              ".log");
    fp_ = ::fopen(DefaultPath.data(), "ae");
  }
}

LogFile::~LogFile() {
  Flush();
  fclose(fp_);
}

void LogFile::Write(const char* data, int len) {
  int pos = 0;
  while (pos != len) {
    pos += static_cast<int>(fwrite_unlocked(data + pos, 
    										sizeof(char), len - pos, fp_));
  }
  time_t now = ::time(nullptr);
  if (len != 0) {
    lastwrite_ = now;
    written_bytes_ += len;
  }
  if (lastwrite_ - lastflush_ > kFlushInterval) {
    Flush();
    lastflush_ = now;
  }
}


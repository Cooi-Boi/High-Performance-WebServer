#ifndef TINY_MUDUO_BUFFER_H_
#define TINY_MUDUO_BUFFER_H_

#include <vector>
#include <algorithm>
#include <string>

#include <assert.h>
#include <cstring>

#include "noncopyable.h"

using std::string;

namespace tiny_muduo {

static const int kPrePendIndex = 8;
static const int kInitialSize = 1024;
static const char* kCRLF = "\r\n";

class Buffer : public NonCopyAble {
 public:
  Buffer() : buffer_(kInitialSize), read_index_(kPrePendIndex), write_index_(kPrePendIndex) {
  }
  ~Buffer() {}

  int ReadFd(int fd);
  
  char* begin() { return &*buffer_.begin(); };
  const char* begin() const { return &*buffer_.begin(); };

  char* beginread() { return begin() + read_index_; } 
  const char* beginread() const { return begin() + read_index_; }

  char* beginwrite() { return begin() + write_index_; }
  const char* beginwrite() const { return begin() + write_index_; }

  const char* FindCRLF() const { 
    const char* find = std::search(Peek(), beginwrite(), kCRLF, kCRLF + 2); 
    return find == beginwrite() ? nullptr : find;
  }

  void Append(const char* message) {
    Append(message, static_cast<int>(strlen(message)));
  }

  void Append(const char* message, int len) {
    MakeSureEnoughStorage(len);
    std::copy(message, message + len, beginwrite());
    write_index_ += len;
  }

  void Append(const string& message) {
    Append(message.data(), static_cast<int>(message.size())); 
  }

  void Retrieve(int len) {
    assert(readablebytes() >= len);
    if (len + read_index_ < write_index_) {
      read_index_ += len;
    } else {
      RetrieveAll();
    }
  }

  void RetrieveUntilIndex(const char* index) {
    assert(beginwrite() >= index);
    read_index_ += static_cast<int>(index - beginread());
  }

  void RetrieveAll() {
    write_index_ = kPrePendIndex;
    read_index_ = write_index_;
  }

  string RetrieveAsString(int len) {
    assert(read_index_ + len <= write_index_);
    string ret = std::move(PeekAsString(len));
    Retrieve(len); 
    return ret;
  }

  string RetrieveAllAsString() {
    string ret = std::move(PeekAllAsString());
    RetrieveAll();
    return ret;
  }

  const char* Peek() const {
    return beginread();
  }

  char* Peek() {
    return beginread();
  }

  string PeekAsString(int len) {
    return string(beginread(), beginread() + len);
  }

  string PeekAllAsString() {
    return string(beginread(), beginwrite()); 
  }
  
  int readablebytes() const { return write_index_ - read_index_; }
  int writablebytes() const { return static_cast<int>(buffer_.size()) - write_index_; } 
  int prependablebytes() const { return read_index_; }

  void MakeSureEnoughStorage(int len) {
    if (writablebytes() >= len) return;
    if (writablebytes() + prependablebytes() >= kPrePendIndex + len) {
      std::copy(beginread(), beginwrite(), begin() + kPrePendIndex);
      write_index_ = kPrePendIndex + readablebytes();
      read_index_ = kPrePendIndex;
    } else {
      buffer_.resize(write_index_ + len);
    }
  }

 private:
  std::vector<char> buffer_;
  int read_index_;
  int write_index_;
};

}
#endif

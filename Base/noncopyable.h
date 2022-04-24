#ifndef TINY_MUDUO_NONCOPYABLE_H_
#define TINY_MUDUO_NONCOPYABLE_H_

namespace tiny_muduo {

class NonCopyAble {
 protected:
  NonCopyAble() {}
  ~NonCopyAble() {}
 
 private:
  NonCopyAble(const NonCopyAble&) = delete;
  NonCopyAble& operator=(const NonCopyAble&) = delete;
};

} // namespace tiny_muduo

#endif

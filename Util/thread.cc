#include "thread.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

#include "latch.h"

using namespace tiny_muduo;

namespace CurrentThread {

__thread int t_cachedTid = 0;
__thread char t_formattedTid[32];
__thread int t_formattedTidLength;

pid_t gettid() {
  return static_cast<int>(syscall(SYS_gettid));
}

void CacheTid() {
  if (t_cachedTid == 0) {
    t_cachedTid = gettid();
    t_formattedTidLength = snprintf(t_formattedTid, sizeof(t_formattedTid), "%5d ", t_cachedTid);
  }
}

} // namespace CurrentThread;

static void* ThreadRun(void* arg) {
  ThreadData* ptr = static_cast<ThreadData*>(arg);
  ptr->RunOneFunc();
  delete ptr;
  return nullptr;
}

Thread::Thread(const ThreadFunc& func, const string& name)
    : pthread_id_(-1),
      func_(func),
      latch_(1),
      name_(std::move(name)) {
}

Thread::~Thread() {
  ::pthread_detach(pthread_id_);
}

void Thread::StartThread() {
  ThreadData* ptr = new ThreadData(func_, &latch_, name_);
  ::pthread_create(&pthread_id_, nullptr, ThreadRun, ptr);
  latch_.Wait();
}

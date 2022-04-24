#include "timer.h"

#include <utility>

using namespace tiny_muduo;

Timer::Timer(Timestamp expiration__, BasicFunc&& cb, double interval = 0.0)
    : expiration_(expiration__),
      callback_(std::move(cb)),
      interval_(interval),
      repeat_(interval > 0.0) {
}

#include "channel.h"

#include <sys/poll.h>
#include <sys/epoll.h>

using namespace tiny_muduo;

Channel::Channel(EventLoop* loop,
                 const int& fd__)
    : loop_(loop),
      fd_(fd__),
      events_(0),
      recv_events_(0),
      tied_(false),
      state_(kNew) {
}

Channel::~Channel() {
}

void Channel::HandleEvent() {
  if (tied_) {
  	std::shared_ptr<void> guard = tie_.lock();
  	HandleEventWithGuard();
  } else {
  	HandleEventWithGuard();
  }
}

void Channel::HandleEventWithGuard() {
  if (recv_events_ & POLLNVAL) {
  	LOG_ERROR << "Channel::HandleEventWithGuard POLLNVAL";
  }
  
  if (recv_events_ & (EPOLLERR | POLLNVAL)) {
  	if (error_callback_) {
  	  error_callback_();
  	}
  }

  if (recv_events_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
    if (read_callback_) {
      read_callback_();
    }
  } 

  if (recv_events_ & EPOLLOUT) {
    if (write_callback_) {
      write_callback_();
    }
  }
}

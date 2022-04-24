#include "epoller.h"

#include <assert.h>
#include <string.h>
#include <sys/epoll.h>

#include <vector>

#include "channel.h"
#include "logging.h"

using namespace tiny_muduo;

Epoller::Epoller()
    : epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kDefaultEvents), 
      channels_() {
}

Epoller::~Epoller() {
  ::close(epollfd_);
}

void Epoller::Poll(Channels& channels) {
  int eventnums = EpollWait();
  FillActiveChannels(eventnums, channels);
}

void Epoller::FillActiveChannels(int eventnums, Channels& channels) {
  for (int i = 0; i < eventnums; ++i) {
    Channel* ptr = static_cast<Channel*> (events_[i].data.ptr);
    ptr->SetReceivedEvents(events_[i].events);
    channels.emplace_back(ptr);
  }
  if (eventnums == static_cast<int>(events_.size())) {
      events_.resize(eventnums * 2);
  }
}

void Epoller::Remove(Channel* channel) {
  int fd = channel->fd();
  ChannelState state = channel->state();
  assert(state == kDeleted || state == kAdded);

  if (state == kAdded) {
    UpdateChannel(EPOLL_CTL_DEL, channel);
  }
  channel->SetChannelState(kNew);
  channels_.erase(fd);
  return;
}

void Epoller::Update(Channel* channel) {
  int op = 0, events = channel->events();
  ChannelState state = channel->state(); 
  int fd = channel->fd();

  if (state == kNew || state == kDeleted) {
    if (state == kNew) {
      assert(channels_.find(fd) == channels_.end());
      channels_[fd] = channel;
    } else {
      assert(channels_.find(fd) != channels_.end());
      assert(channels_[fd] == channel);
    }
    op = EPOLL_CTL_ADD;
    channel->SetChannelState(kAdded);
  } else {
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    if (events == 0) {
      op = EPOLL_CTL_DEL;
      channel->SetChannelState(kDeleted); 
    } else {
      op = EPOLL_CTL_MOD;
    }
  }
  
  UpdateChannel(op, channel);
}

void Epoller::UpdateChannel(int operation, Channel* channel) {
  struct epoll_event event;
  event.events = channel->events();
  event.data.ptr = static_cast<void*>(channel);

  if (epoll_ctl(epollfd_, operation, channel->fd(), &event) < 0) {
      LOG_ERROR << "Epoller::UpdataChannel epoll_ctl failed"; 
  }
  return;
}

#include "tcpconnection.h"

#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#include "channel.h"
#include "buffer.h"
#include "logging.h"

using namespace tiny_muduo;

TcpConnection::TcpConnection(EventLoop* loop__, int connfd, int id__) 
    : loop_(loop__),
      fd_(connfd),
      connection_id_(id__),
      state_(kConnecting),
      channel_(new Channel(loop_, fd_)),
      shutdown_state_(false) {
  channel_->SetReadCallback(std::bind(&TcpConnection::HandleMessage, this));
  channel_->SetWriteCallback(std::bind(&TcpConnection::HandleWrite, this));
  channel_->SetErrorCallback(std::bind(&TcpConnection::HandleError, this));
}

TcpConnection::~TcpConnection() {
  ::close(fd_);
}

void TcpConnection::ConnectionDestructor() {
  if (state_ == kDisconnecting || state_ == kConnected) {
    state_ = kDisconnected;
    channel_->DisableAll();
  }
  loop_->Remove(channel_.get());
}

int TcpConnection::GetErrno() const {
  int optval;
  socklen_t optlen = static_cast<socklen_t>(sizeof optval);

  if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
    return errno;
  } else {
    return optval;
  }
}

void TcpConnection::HandleClose() {
  state_ = kDisconnected;
  channel_->DisableAll();

  TcpConnectionPtr guard(shared_from_this());
  close_callback_(guard);
}


void TcpConnection::HandleError() {
  LOG_ERROR << "TcpConnection::HandleError" << " : " << ErrorToString(GetErrno());
}


void TcpConnection::HandleMessage() {
  int read_size = input_buffer_.ReadFd(fd_);
  if (read_size > 0) {
    message_callback_(shared_from_this(), &input_buffer_);
  } else if (read_size == 0) {
    HandleClose();
  } else {
  	LOG_ERROR << "TcpConnection::HandleMessage read failed";
  }
}

void TcpConnection::HandleWrite() {
  if (channel_->IsWriting()) {
    int len = output_buffer_.readablebytes();
    int remaining = len;
    int send_size = static_cast<int>(::write(fd_, output_buffer_.Peek(), remaining));
    if (send_size < 0) {
      assert(send_size >= 0);
      if (errno != EWOULDBLOCK) {
        LOG_ERROR << "TcpConnection::HandleWrite write failed";
      }
      return;
    }
    remaining -= send_size;
    output_buffer_.Retrieve(send_size);
  
    assert(remaining <= len);
    if (!output_buffer_.readablebytes()) {
      channel_->DisableWriting();
      if (state_ == kDisconnecting) {
        Shutdown();
      }
    }
  }
}

// Only Can Send In Same Thread, Can't Cross-Thread
void TcpConnection::Send(const char* message, int len) {
  int remaining = len;
  int send_size = 0;
  if (!channel_->IsWriting() && output_buffer_.readablebytes() == 0) {
    send_size = static_cast<int>(::write(fd_, message, len));
    if (send_size >= 0) {
      remaining -= send_size; 
    } else {
      if (errno != EWOULDBLOCK) {
        LOG_ERROR << "TcpConnection::Send write failed";
      }
      return;
    }
  }

  assert(remaining <= len);
  if (remaining > 0) {
    output_buffer_.Append(message + send_size, remaining);
    if (!channel_->IsWriting()) {
      channel_->EnableWriting(); 
    }
  }
}

void TcpConnection::Shutdown() { 
  state_ = kDisconnecting;
  if (!channel_->IsWriting()) {
    int ret = ::shutdown(fd_, SHUT_WR);
    if (ret < 0) {
      LOG_ERROR << "TcpConnection::Shutdown shutdown failed";
    }
  }
}

// Only Can Send In Same Thread, Can't Cross-Thread
void TcpConnection::Send(Buffer* buffer) {
  if (state_ == kDisconnected) return; 
  Send(buffer->Peek(), buffer->readablebytes()); 
  buffer->RetrieveAll();
}

// Only Can Send In Same Thread, Can't Cross-Thread
void TcpConnection::Send(const string& message) {
  if (state_ == kDisconnected) return; 
  Send(message.data(), static_cast<int>(message.size()));
}

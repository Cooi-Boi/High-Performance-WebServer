#include "tcpserver.h"

#include <assert.h>

#include <climits>
#include <utility>

#include "eventloopthreadpool.h"
#include "acceptor.h"
#include "tcpconnection.h"

using namespace tiny_muduo;

TcpServer::TcpServer(EventLoop* loop, const Address& address)
    : loop_(loop),
      next_connection_id_(1),
      threads_(new EventLoopThreadPool(loop_)),
      acceptor_(new Acceptor(loop_, address)),
      ipport_(std::move(address.IpPortToString())) {
  acceptor_->SetNewConnectionCallback(std::bind(&TcpServer::HandleNewConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
  for (auto& pair : connections_) {
    TcpConnectionPtr ptr(pair.second);
    pair.second.reset();
    ptr->loop()->RunOneFunc(std::bind(&TcpConnection::ConnectionDestructor, ptr));
  }
}

inline void TcpServer::HandleClose(const TcpConnectionPtr& ptr) {
  loop_->RunOneFunc(std::bind(&TcpServer::HandleCloseInLoop, this, ptr));
}

inline void TcpServer::HandleCloseInLoop(const TcpConnectionPtr& ptr) {
  assert(connections_.find(ptr->fd()) != connections_.end());
  connections_.erase(connections_.find(ptr->fd()));
  LOG_INFO << "TcpServer::HandleCloseInLoop - remove connection " << "[" 
           << ipport_ << '#' << ptr->id() << ']';

  EventLoop* loop = ptr->loop(); 
  loop->QueueOneFunc(std::bind(&TcpConnection::ConnectionDestructor, ptr));
}

inline void TcpServer::HandleNewConnection(int connfd, const Address& address) {
  EventLoop* loop = threads_->NextLoop(); 
  TcpConnectionPtr ptr(new TcpConnection(loop, connfd, next_connection_id_));
  connections_[connfd] = ptr;

  ptr->SetConnectionCallback(connection_callback_);
  ptr->SetMessageCallback(message_callback_);
  ptr->SetCloseCallback(std::bind(&TcpServer::HandleClose, this, _1));
  LOG_INFO << "TcpServer::HandleNewConnection - new connection " << "[" 
           << ipport_ << '#' << next_connection_id_ << ']' << " from " 
           << address.IpPortToString();
  ++next_connection_id_;
  if (next_connection_id_ == INT_MAX) next_connection_id_ = 1;
  loop->RunOneFunc(std::bind(&TcpConnection::ConnectionEstablished, ptr));
}

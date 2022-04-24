#ifndef TINY_MUDUO_TCPSERVER_H_
#define TINY_MUDUO_TCPSERVER_H_

#include <memory>
#include <map>
#include <utility>
#include <memory>
#include <string>

#include "callback.h"
#include "eventloop.h"
#include "acceptor.h"
#include "eventloopthreadpool.h"
#include "tcpconnection.h"
#include "logging.h"
#include "noncopyable.h"
#include "address.h"

namespace tiny_muduo {

class Address;

class TcpServer : public NonCopyAble {
 public:
  TcpServer(EventLoop* loop, const Address& address);
  ~TcpServer();

  void Start() {
    threads_->StartLoop();
    loop_->RunOneFunc(std::bind(&Acceptor::Listen, acceptor_.get()));
  }

  void SetConnectionCallback(ConnectionCallback&& callback) { 
    connection_callback_ = std::move(callback);
  }

  void SetConnectionCallback(const ConnectionCallback& callback) { 
    connection_callback_ = callback;
  }

  void SetMessageCallback(MessageCallback&& callback) {
    message_callback_ = std::move(callback);
  }

  void SetMessageCallback(const MessageCallback& callback) {
    message_callback_ = callback;
  }

  void SetThreadNums(int thread_nums) {
    threads_->SetThreadNums(thread_nums);    
  }

  inline void HandleClose(const TcpConnectionPtr& conn);
  inline void HandleCloseInLoop(const TcpConnectionPtr& ptr);
  inline void HandleNewConnection(int connfd, const Address& address);

 private:
  typedef std::map<int, TcpconnectionPtr> ConnectionMap;

  EventLoop* loop_;
  int next_connection_id_;
  std::unique_ptr<EventLoopThreadPool> threads_;
  std::unique_ptr<Acceptor> acceptor_;
  const std::string ipport_;

  ConnectionCallback connection_callback_;
  MessageCallback message_callback_;
  ConnectionMap connections_;
};

}// namespace tiny_muduo
#endif

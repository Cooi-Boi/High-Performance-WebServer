#include "acceptor.h"

#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <bits/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <functional>

#include <cstring>

#include "address.h"
#include "channel.h"
#include "logging.h"

using namespace tiny_muduo;

Acceptor::Acceptor(EventLoop* loop, const Address& address)
    : loop_(loop),
      listenfd_(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP)),
      idlefd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)),
      channel_(new Channel(loop_, listenfd_)) {
  SetSockoptReuseAddr(listenfd_);
  SetSockoptKeepAlive(listenfd_);
  BindListenFd(address);
  channel_->SetReadCallback(std::bind(&Acceptor::NewConnection, this));
}

Acceptor::~Acceptor() {
  channel_->DisableAll();
  loop_->Remove(channel_.get());
  ::close(listenfd_);
}

void Acceptor::BindListenFd(const Address& addr) {
  struct sockaddr_in address;
  bzero((char*)&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY); // FIXME : it should be addr.ip() and need some conversion
  address.sin_port = htons(static_cast<uint16_t>(addr.port()));
  int ret = bind(listenfd_, (struct sockaddr*)(&address), sizeof(address));
  assert(ret != -1); 
  (void)ret;
}

void Acceptor::Listen() {
  int ret = listen(listenfd_, SOMAXCONN);
  assert(ret != -1);
  (void)ret;
  channel_->EnableReading(); 
}

void Acceptor::NewConnection() {
  struct sockaddr_in client, peeraddr;
  socklen_t client_addrlength = sizeof(client);
  int connfd = ::accept4(listenfd_, (struct sockaddr*)&client, &client_addrlength, SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (connfd < 0) {
    if (errno == EMFILE) {
      ::close(idlefd_);
      idlefd_ = ::accept(listenfd_, NULL, NULL);
      ::close(idlefd_);
      idlefd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
    return;
  }
  assert(connfd > 0);
  if (SetSockoptKeepAlive(connfd) == -1) {
    LOG_ERROR << "Acceptor::NewConnection SetSockoptKeepAlive failed";
    close(connfd);
    return;
  }
  
  if (SetSockoptTcpNoDelay(connfd) == -1) {
    LOG_ERROR << "Acceptor::NewConnection SetSockoptTcpNoDelay failed";
    close(connfd);
    return;
  }

  socklen_t peer_addrlength = sizeof(peeraddr);
  getpeername(connfd, (struct sockaddr *)&peeraddr, &peer_addrlength);
  new_connection_callback_(connfd, Address(inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port)));
}

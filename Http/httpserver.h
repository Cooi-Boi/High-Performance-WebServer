#ifndef TINY_MUDUO_HTTPSERVER_H_
#define TINY_MUDUO_HTTPSERVER_H_

#include <stdio.h>

#include <functional>
#include <utility>
#include <memory>

#include "callback.h"
#include "eventloop.h"
#include "tcpserver.h"
#include "tcpconnection.h"
#include "buffer.h"
#include "httpcontent.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "timestamp.h"
#include "logging.h"

using tiny_muduo::HttpStatusCode;

namespace tiny_muduo {

static const double kIdleConnectionTimeOuts = 8.0;

class HttpServer {
 typedef std::function<void (const HttpRequest&, HttpResponse&)> HttpResponseCallback;

 public:
  HttpServer(EventLoop* loop, const Address& address, 
             bool auto_close_idleconnection = false);
  ~HttpServer();

  void Start() {
    server_.Start();
  }

  void HttpDefaultCallback(const HttpRequest& request, HttpResponse& response) {
    response.SetStatusCode(k404NotFound);
    response.SetStatusMessage("Not Found");
    response.SetCloseConnection(true);
    (void)request;
  }

  void HandleIdleConnection(std::weak_ptr<TcpConnection>& connection) {
    TcpConnectionPtr conn(connection.lock());
    if (conn) {
      if (Timestamp::AddTime(conn->timestamp(), kIdleConnectionTimeOuts) < Timestamp::Now()) {
        conn->Shutdown();
      } else {    
        loop_->RunAfter(kIdleConnectionTimeOuts, 
                        std::move(std::bind(&HttpServer::HandleIdleConnection,
                        this, connection))); 
      }
    }
  }

  void ConnectionCallback(const TcpConnectionPtr& connection) {
    if (auto_close_idleconnection_) {
      loop_->RunAfter(kIdleConnectionTimeOuts, 
                      std::bind(&HttpServer::HandleIdleConnection, 
                      this, std::weak_ptr<TcpConnection>(connection))); 
    }
  }

  void MessageCallback(const TcpConnectionPtr& connection, Buffer* buffer);

  void SetHttpResponseCallback(const HttpResponseCallback& response_callback) { 
    response_callback_ = response_callback; 
  }

  void SetHttpResponseCallback(HttpResponseCallback&& response_callback) { 
    response_callback_ = std::move(response_callback); 
  } 
  
  void SetThreadNums(int thread_nums) {
  	server_.SetThreadNums(thread_nums);
  }

  void DealWithRequest(const HttpRequest& request, const TcpConnectionPtr& connection);

 private:
  EventLoop* loop_;
  TcpServer server_;
  bool auto_close_idleconnection_;

  HttpResponseCallback response_callback_;
};

}

#endif

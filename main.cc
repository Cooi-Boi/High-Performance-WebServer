#include <stdio.h>

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <climits>

#include "eventloop.h"
#include "address.h"
#include "httpserver.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "httpresponsefile.h"
#include "logging.h"
#include "asynclogging.h"

using namespace tiny_muduo;

using tiny_muduo::Method;
using tiny_muduo::HttpStatusCode;

std::unique_ptr<AsyncLogging> asynclog;
extern void SetOutputFunc(Logger::OutputFunc);
extern void SetFlushFunc(Logger::FlushFunc);

void AsyncOutputFunc(const char* data, int len) {
  asynclog->Append(data, len);
}

void AsyncFlushFunc() {
  asynclog->Flush();
}

void HttpResponseCallback(const HttpRequest& request, HttpResponse& response) {
  if (request.method() != kGet) {
    response.SetStatusCode(k400BadRequest);
    response.SetStatusMessage("Bad Request");
    response.SetCloseConnection(true);
    return;
  }    
  
  {
    const string& path = request.path(); 
    if (path == "/") {
      response.SetStatusCode(k200OK);
      response.SetBodyType("text/html");
      response.SetBody(love6_website);
    } else if (path == "/hello") {
      response.SetStatusCode(k200OK);
      response.SetBodyType("text/html");
      response.SetBody("Hello, world!\n");
    } else if (path == "/favicon.ico" || path == "/favicon") {
      response.SetStatusCode(k200OK);
      response.SetBodyType("image/png");
      response.SetBody(string(favicon, sizeof(favicon))); 
    } else {
      response.SetStatusCode(k404NotFound);
      response.SetStatusMessage("Not Found");
      response.SetCloseConnection(true);
      return;
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    printf("Usage: %s port\n", argv[0]);
    return 0;
  }

  // NOTE : async logging
  // asynclog.reset(new AsyncLogging());
  // SetOutputFunc(AsyncOutputFunc);
  // SetFlushFunc(AsyncFlushFunc);
  // asynclog->StartAsyncLogging();

  // NOTE : for benchmark
  // SetLogLevel(Logger::Level::ERROR);
  
  EventLoop loop;
  Address listen_address(argv[1]);
  HttpServer server(&loop, listen_address);
  server.SetThreadNums(4);
  server.SetHttpResponseCallback(HttpResponseCallback);
  server.Start();
  loop.Loop();
  
  return 0;
}

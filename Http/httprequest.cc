#include "httprequest.h"

#include <utility>
#include <algorithm>
#include <string>

#include "httpparsestate.h"

using namespace tiny_muduo;
using tiny_muduo::Method;
using tiny_muduo::HttpRequestParseState;

HttpRequest::HttpRequest() {}

HttpRequest::~HttpRequest() {}

bool HttpRequest::ParseRequestMethod(const char* start, const char* end) {
  string method(start, end);
  bool has_method = true;
  if (method == "GET") {
    method_ = kGet;
  } else if (method == "POST") {
    method_ = kPost;
  } else if (method == "PUT") {
    method_ = kPut;
  } else if (method == "DELETE") {
    method_ = kDelete;
  } else if (method == "TRACE") {
    method_ = kTrace;
  } else if (method == "OPTIONS") {
    method_ = kOptions;
  } else if (method == "CONNECT") {
    method_ = kConnect;
  } else if (method == "PATCH") {
    method_ = kPatch;
  } else {
    has_method = false;
  }

  return has_method;
}

bool HttpRequest::ParseRequestLine(const char* start, const char* end) {
  const char* space = nullptr;     
  space = std::find(start, end, ' ');
  if (space == end) {
    return false;
  }
  
  if (!ParseRequestMethod(start, space)) {
    return false;
  }
  start = space + 1; 
 
  space = std::find(start, end, ' ');
  if (space == end) {
    return false;
  }

  const char* query = std::find(start, end, '?'); 
  if (query != end) {
    path_.assign(start, query);
    query_.assign(query + 1, space);
  } else {
    path_.assign(start, space);
  }        
  start = space + 1;
  
  bool parsehttp = (start + 8 == end) && std::equal(start, end - 1, http);
  if (!parsehttp || (*(end - 1) != '0' && *(end - 1) != '1')) {
    version_ = kUnknown;
    return false;
  }

  if (*(end - 1) == '0') {
    version_ = kHttp10;
  } else {
    version_ = kHttp11; 
  }

  return true;
}

bool HttpRequest::ParseBody(const char* start, const char* end) { 
  (void)start;
  (void)end;
  return true; 
}

bool HttpRequest::ParseHeaders(const char* start, const char* colon, const char* end) {
  const char* vaildstart = colon + 1; 
  while(*vaildstart == ' ') { ++vaildstart; }
  headers_[std::move(string(start, colon))] = std::move(string(vaildstart, end));
  return true;
}

void HttpRequest::Swap(HttpRequest& req) {
   method_ = req.method_;
   version_ = req.version_;
   path_.swap(req.path_);
   query_.swap(req.query_);
   headers_.swap(req.headers_);
}

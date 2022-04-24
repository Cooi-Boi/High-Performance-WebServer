#ifndef TINY_MUDUO_HTTPSTATE_H_
#define TINY_MUDUO_HTTPSTATE_H_

namespace tiny_muduo {

enum HttpRequestParseState {
  kParseRequestLine,
  kParseHeaders,
  kParseBody,
  kParseGotCompleteRequest,
  kParseErrno,
};

}

#endif

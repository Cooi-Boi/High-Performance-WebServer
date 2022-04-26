#ifndef TINY_MUDUO_ADDRESS_H_
#define TINY_MUDUO_ADDRESS_H_

#include <stdio.h>

#include <string>
#include <cstring>

namespace tiny_muduo {

class Address {
 private:
  const char* ip_;
  const int port_;

 public:
  Address(const char* port__ = "80") : ip_("0.0.0.0"), port_(atoi(port__)) {}
  Address(const int port__ = 80) : ip_("0.0.0.0"), port_(port__) {}
  Address(const char* ip__, const char* port__) 
      : ip_(ip__), port_(atoi(port__)) {}
  Address(const char* ip__, const int port__) 
      : ip_(ip__), port_(port__) {}

  const char* ip() const { return ip_; }
  int port() const { return port_; }
  const std::string IpPortToString() const {
  	char buf[32];
  	int len = snprintf(buf, sizeof(buf), "%s:%d", ip_, port_);
  	return std::string(buf, buf + len);
  }
};

}
#endif

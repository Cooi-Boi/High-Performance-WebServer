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
  Address(const char* port = "2022") : ip_("0.0.0.0"), port_(atoi(port)) {}
  Address(const int port = 2022) : ip_("0.0.0.0"), port_(port) {}
  Address(const char* ip, const char* port) 
      : ip_(ip), port_(atoi(port)) {}
  Address(const char* ip, const int port) 
      : ip_(ip), port_(port) {}

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

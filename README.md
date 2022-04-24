# High-Performance-WebServer

这个项目是基于C++11编写的Web服务器，以事件驱动模式为核心。

WebServer解析HTTP头部支持解析GET命令，可支持长连接。

WebServer实现了定时断开空闲连接（基于内部定时器实现），异步高性能日志库完成服务器Logging功能。

## 环境

支持Ubuntu/CentOS/Debian等
编写环境于Ubuntu 20.04

## 依赖

g++ 
cmake 
make

## 安装必要软件：

**Debian, Ubuntu, etc.**

  sudo apt install g++ cmake make
    
**CentOS**

  sudo yum install gcc-c++ cmake make

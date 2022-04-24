# High-Performance-WebServer

本项目是基于C++11编写的Web服务器，整个服务器是以事件驱动模式为核心。  
支持解析HTTP头部，响应HTTP GET请求，可支持服务器长连接。  
实现了服务器定时断开空闲连接（基于内部定时器实现），异步高性能日志库完成服务器Logging功能。  

## Ststem Requires:

  支持Ubuntu/CentOS/Debian, etc.  
  编写环境于Ubuntu 20.04

## Packages Required:

  g++  
  cmake  
  make  

  **Debian, Ubuntu, etc.**  
  sudo apt install g++ cmake make
    
  **CentOS**  
  sudo yum install gcc-c++ cmake make
  
## To Build

	./build.sh

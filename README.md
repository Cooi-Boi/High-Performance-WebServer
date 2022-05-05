# High-Performance-WebServer

本项目是基于C++11编写的Web服务器，整个服务器是以事件驱动模式为核心。  
支持解析HTTP头部，响应HTTP GET请求，可支持服务器长连接。  
实现了服务器定时断开空闲连接（基于内部定时器实现），异步高性能日志库完成服务器Logging功能。  


## Process Records

  **[Love 6's CSDN Blog](https://love6.blog.csdn.net)**  
  **[实现C++ High-Performance WebServer全流程记录（基于muduo网络库）](https://love6.blog.csdn.net/article/details/123754194)**
  
## System Requires:

  Linux kernel version >= 2.6.28.

  支持Ubuntu/CentOS/Debian, etc.  
  编写环境于Ubuntu 20.04 g++ 4.8.5

## Packages Required:

  `g++ >= 4.8.5`  
  `cmake`  
  `make`  

  **Debian, Ubuntu, etc.**  
  `sudo apt install g++ cmake make`
    
  **CentOS**  
  `sudo yum install gcc-c++ cmake make`
  
## To Build

	./build.sh
	
## Optimized Methods
  
  * 采用Epoll I/O多路复用工作函数，工作模式采用LT水平触发模式，对长连接短连接处理性能友好  
  * 采用RAII机制，智能指针Shared/Unique Ptr来管理内存，避免手动delete，同时也避免了内存泄漏  
  * 采用线程池管理线程，并在程序开始时产生线程，避免程序运行中产生影响性能，并具有可预测性的性能  
  * 采用DOUBLE-BUFFERING双缓冲机制，自定义固定大小Buffer并重载运算符，优化异步日志库写入性能  
  * 采用Eventfd作为事件通知描述符，效率高且非常简单的通信工具，优化主线程和子线程间的事件通知效率  
  * 采用Timerfd作为定时器触发描述符，红黑树作为定时器管理基本数据结构，高效的管理定时器触发事件，完美的融入了事件驱动管理  
  * 采用__thread gcc内置线程局部存储设施，存储日期/PID/时间，节省了多次系统调用，进一步优化日志库Logging效率  
  * 采用优雅断开服务器连接方式，避免直接Close而丢失双方仍未发出或接受的数据  
  * 采用C++11 std::move移动函数避免大量不必要的拷贝动作  
  * 采用提前准备一个空的文件描述符，在面对服务器文件描述符耗尽时关闭/接受/再关闭的方法，有效避免因描述符耗尽而导致服务器Busy Loop  
  * 采用有限状态机分析TCP字节流是否为HTTP包，高效的解析HTTP请求中的URI以及HTTP头部参数，采用unordered_map来记录参数方便使用  
  * WebServer采用One Loop Per Thread + Thread Pool编程模式，其内部实现了一个小型Reactor模式的网络库
  
## Code Statistics
  ![image](https://user-images.githubusercontent.com/72536813/164964817-6aa10adb-0519-411c-982d-c853181f89cb.png)
  
## Thanks For
  >@linyacool 感谢`linya`学长在网上发的经验分享，也是才我让了解到有这个项目，其中很多地方，包括Webbench这个工具还有一些其他的小工具，我也都从Linya学长所写的项目中学习使用到的。  
  还有以及这个README，linya学长所写的真的很美观，我又写不出来，格式很多地方也是借鉴的，希望能够原谅我，哈哈。真心很感谢linya学长！  
  >
  >这个项目，最上面的链接里面，有我从零开始记录，一点点什么都不懂到完成这样一个项目的全过程。在一开始我直接看muduo那本书真的发现 我真的很多东西一点都看不懂，对于这个项目怎么入手也不清楚。之后索性就书籍为辅，源码为主，一头扎进源码里面开始研究。在实现遇到疑惑不解的时候，才会跳到书籍中相关的地方去看看能不能找到解答。  
  >在最开始的实现过程中，我都是先用纸记录，帮助我理清muduo的运行轨迹。在彻底分析清运行流程后，我就会关掉源码和书籍，自己凭借着记忆与理解去实现，在左思右想抓耳络腮都还是想不出来怎么实现的时候，再回头看看源码，在分析完的一声惊呼后再回来继续写。  
  >
  >从开始的麻雀虽小，但好在一步步都在往前走，从最开始实现一个40-50行的满是错误仅仅能支持单行回复的`echo`小程序，到后面能够以单线程`Reactor`接受多个客户的`echo`后，再到后面实现一个进程池全是内存泄漏但算是多进程的`echo`服务器，再到后面才来到分析`muduo`，一切推翻重新开始，开始进入正式写`muduo`....。
  >
  >在实现的过程中，我遇到了各自各样的困难和疑惑，有些很快就解决了，有些则是一解决就花了我四五天。  
  有的时候遇到一些几天都解决不了的问题，真的还是挺让人垂头丧气的，在中途甚至有过放弃继续写下去，性能瓶颈就不找了的想法，但一路走过来到现在，还算好，困难与挫折都克服了，走到了现在这一步，这个项目也算是完工了。
  >
  >一路走过来的轨迹 从零开始的记录我也都一五一十的记录在了我的博客里面 也就是最上方的链接。  
  我写博客的意义 就是希望能给一些 想要走后台开发的路线 可能也像之前我一样 想要做一个类似项目 却一点思路和头绪都没有的同学 一些想法和启发  
  再次感谢`linya`学长 带我入了门 也再次感谢`muduo` 一路上收获的太多 也算是带我真正意义上走进了后台开发领域的第一步  
  还有太多太多想说的了 这里就不写了 多的留在了博客里面 希望我写的博客与项目 可以给还在路上的各位一些启发  
  
  >就写到这里吧 希望我写的这些对各位有些许帮助。  
  Thanks For Visiting~ 有缘大家江湖再见！ヾ(￣▽￣)Bye\~Bye\~～

#ifndef QINGYI_NET_SOCKETSOPS_H
#define QINGYI_NET_SOCKETSOPS_H

#include <arpa/inet.h>

namespace qingyi{
namespace net{
namespace sockets{

//create a nonblocking socket file descriptor
//abort if any error happen

//typedef unsigned short sa_family_t 2bytes

int CreateNonBlockingOrDie(sa_family_t family);

// struct sockaddr{
//   sa_family_t family;
//   char sa_data[14];
// }     //totally 16 bytes
int Connect(int sockfd, const struct sockaddr* addr);

void BindOrDie(int sockfd, const struct sockaddr* addr);

void ListenOrDie(int sockfd);

// in_port_t an unsigned integral type of exactly 16 bits
// in_addr_t an unsigned integral type of exactly 32 bits

//struct in6_addr{   //16 bytes
//  union{
//    uint8_t __u6_addr8[16];
//    uint16_t __u6_addr16[8];
//    uint32_t __u6_addr32[4];
//  }
//}

// struct sockaddr_in6 {  //total 28 bytes;
//   sa_family_t family;  //unsigned short 2 bytes
//   in_port_t sin6_port; // 2 bytes
//   uint32_t sin6_flowinfo; //4 bytes
//   struct in6_addr sin6_addr; //16 bytes IPV6 16 字节表示IP地址
//   uint32_t sin6_scope_id;  //4bytes
// }


//struct in_addr {
//  in_addr_t s_addr; //4 bytes typedef uint32_t in_addr_t
//}

// struct sockaddr_in { // 16 bytes
//   sa_family_t sin_family; //2 bytes
//   uint16_t sin_port; //2 bytes
//   struct in_addr sin_addr; //4 bytes
//   char sin_zert[8]; // 8 bytes
// }

//其实sockaddr 和 sockaddr_in 之间的转化很容易理解，因为他们开头一样，内存大小也一样，但
//是sockaddr和sockaddr_in6之间的转换就有点让人搞不懂了，其实你有可能被结构所占的内存迷惑了
//，这几个结构在作为参数时基本上都是以指针的形式传入的，我们拿函数bind()为例，这个函数一共接
//收三个参数，第一个为监听的文件描述符，第二个参数是sockaddr*类型，第三个参数是传入指针原结
//构的内存大小，所以有了后两个信息，无所谓原结构怎么变化，因为他们的头都是一样的，也就是
//uint16 sa_family，那么我们也能根据这个头做处理

int Accept(int sockfd, struct sockaddr_in6* addr);

ssize_t Read(int sockfd, void *buf, size_t count);

ssize_t Readv(int sockfd, const struct iovec *iov, int iovcnt);

ssize_t Write(int sockfd, const void *buf, size_t count);

void Close(int sockfd);

void ShutdownWrite(int sockfd);

void ToIpPort(char* buf, size_t size,
              const struct sockaddr* addr);

void ToIp(char* buf, size_t size,
          const struct sockaddr* addr);

void FromIpPort(const char* ip, uint16_t port,
                struct sockaddr_in* addr);

void FromIpPort(const char* ip, uint16_t port,
                struct sockaddr_in6* addr);

int GetSocketError(int sockfd);

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);

const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);

struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);

struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);

const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);

const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);

struct sockaddr_in6 GetLocalAddr(int sockfd);

struct sockaddr_in6 GetPeerAddr(int sockfd);

bool IsSelfConnect(int sockfd);

}//end of namespace sockets
}//end of namespace net
}//end of namespace qingyi
#endif//QINGYI_NET_SOCKOPTS_H

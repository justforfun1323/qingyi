#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/uio.h>  // readv
#include <unistd.h>
#include <assert.h>

#include "qingyi/net/socketsopts_.h"
#include "qingyi/util/type_.h"
#include "qingyi/net/endian_.h"

using namespace qingyi;
using namespace qingyi::net;

namespace {  //无名命名空间，不需要空间均可以看到，
           //类似于全局  命名空间作用域局限在当前文件中
typedef struct sockaddr SA; //under namespace of qingyi::net;


void SetNonBlockAndCloseOnExec(int sockfd) {
  //fcntl系统调用可以用来对已打开的文件描述符进行各种控制操作以改变已打开文件的的各种属性
  int flag = ::fcntl(sockfd, F_GETFL, 0);
  flag |= O_NONBLOCK;
  int res = ::fcntl(sockfd, F_SETFL, flag);
  
  //close-on-exec
  flag = ::fcntl(sockfd, F_GETFD, flag); 
  flag |= FD_CLOEXEC;
  int ret = ::fcntl(sockfd, F_SETFD, flag); //在子进程中自动关闭这个端口号
  //(void) ret;
}

}// namespace 

//我个人理解，转换可以成立主要是因为是指针的转换，利用sa_family_t的类型来确定实际大小，
//在执行操作时候，转换为实际大小
const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in6* addr) {
  return reinterpret_cast<const struct sockaddr*> (addr);
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in6* addr) {
  return reinterpret_cast<struct sockaddr*> (addr);
}

const struct  sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr) {
  return reinterpret_cast<const struct sockaddr*> (addr);
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in* addr) {
  return reinterpret_cast<struct sockaddr*> (addr);
}

const struct sockaddr_in* sockets::sockaddr_in_cast(const struct sockaddr* addr) {
  return reinterpret_cast<const struct sockaddr_in*> (addr);
}

const struct sockaddr_in6* sockets::sockaddr_in6_cast(const struct sockaddr* addr) {
  return reinterpret_cast<const struct sockaddr_in6*> (addr);
}

int sockets::CreateNonBlockingOrDie(sa_family_t family) {
  int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
  if (sockfd < 0) {
    //log error  
    return -1;
  }
  SetNonBlockAndCloseOnExec(sockfd);
  return sockfd;
}

void sockets::BindOrDie(int sockfd, const struct sockaddr* addr) {
  int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(sockaddr)));
  if (ret < 0) {
    //log error
  }
}

void sockets::ListenOrDie(int sockfd) {
  int ret = ::listen(sockfd, SOMAXCONN);
  if (ret < 0) {
    //log error
  }
}

int sockets::Accept(int sockfd, struct sockaddr_in6* addr) {
  socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr)); //可能为ip6
  //需要知道实际它的长度
  int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
  SetNonBlockAndCloseOnExec(connfd);
  if (connfd < 0) {
    //log error
  }
  return connfd;
}

int sockets::Connect(int sockfd, const struct sockaddr* addr) {
  return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

ssize_t sockets::Read(int sockfd, void *buf, size_t count) {
  return ::read(sockfd, buf, count);
}

ssize_t sockets::Readv(int sockfd, const struct iovec *iov, int iovcnt) {
  return ::readv(sockfd, iov, iovcnt);
}

ssize_t sockets::Write(int sockfd, const void *buf, size_t count) {
  return ::write(sockfd, buf, count);
}

void sockets::Close(int sockfd) {
  if (::close(sockfd) < 0) {
    //log error
  }
}

void sockets::ShutdownWrite(int sockfd) {
  if (::shutdown(sockfd, SHUT_WR) < 0) {
    //log error
  }
}

void sockets::ToIpPort(char* buf, size_t size,
                          const struct sockaddr* addr) {
  ToIp(buf, size, addr);
  size_t end = ::strlen(buf);
  const struct sockaddr_in* addr4 = sockaddr_in_cast(addr); 
  uint16_t port = sockets::NetworkToHost16(addr4->sin_port);
  assert(size > end);
  snprintf(buf + end, size - end, ":%u", port);
}

void sockets::ToIp(char* buf, size_t size,
			  const struct sockaddr* addr) {
  if (addr->sa_family == AF_INET) {
    assert(size >= INET_ADDRSTRLEN);
    const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
    ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
    //用于点十分进制和二进制之间IP地址的转换
  }
  else if (addr->sa_family == AF_INET6) {
    assert(size >= INET6_ADDRSTRLEN);
    const struct sockaddr_in6* addr6 = sockaddr_in6_cast(addr);
    ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
  }
}

void sockets::FromIpPort(const char* ip, uint16_t port, 
                         struct sockaddr_in* addr) {
  addr->sin_family = AF_INET;
  addr->sin_port = HostToNetwork16(port);
  if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
    //log error
  }
}

void sockets::FromIpPort(const char* ip, uint16_t port,
                         struct sockaddr_in6* addr) {
  addr->sin6_family = AF_INET6;
  addr->sin6_port = HostToNetwork16(port);
  if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0) {
    //log error
  }
}

int sockets::GetSocketError(int sockfd) {
  int optval;
  socklen_t optlen = static_cast<socklen_t> (sizeof(optval));
  //获取任意类型，任意状态套接字选项当前值，并且存入optval
  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
    return errno;
  }
  else {
    return optval;
  }
}

struct sockaddr_in6 sockets::GetLocalAddr(int sockfd) {
  struct sockaddr_in6 addr6;
  MemZero(&addr6, sizeof(addr6));
  socklen_t addrlen = static_cast<socklen_t>(sizeof(addr6));
  if (::getsockname(sockfd, sockaddr_cast(&addr6), &addrlen)) {
    //log error
  }
  return addr6;
}

struct sockaddr_in6 sockets::GetPeerAddr(int sockfd) {
  struct sockaddr_in6 addr6;
  MemZero(&addr6, sizeof(addr6));
  socklen_t addrlen = static_cast<socklen_t>(sizeof(addr6));
  if (::getpeername(sockfd, sockaddr_cast(&addr6), &addrlen)) {
    //log error
  }
  return addr6;
}

bool sockets::IsSelfConnect(int sockfd) {
  struct sockaddr_in6 localaddr = GetLocalAddr(sockfd);
  struct sockaddr_in6 peeraddr = GetPeerAddr(sockfd);
  if (localaddr.sin6_family == AF_INET) {
    const struct sockaddr_in* localaddr4 = reinterpret_cast<sockaddr_in*> (&localaddr);
    const struct sockaddr_in* peeraddr4 = reinterpret_cast<sockaddr_in*> (&peeraddr);
    return localaddr4->sin_port == peeraddr4->sin_port
          && localaddr4->sin_addr.s_addr == peeraddr4->sin_addr.s_addr;
  }
  else if (localaddr.sin6_family == AF_INET6) {
    return localaddr.sin6_port == peeraddr.sin6_port 
           && memcmp(&localaddr, &peeraddr, sizeof(localaddr)) == 0;
  }
  return false;
}

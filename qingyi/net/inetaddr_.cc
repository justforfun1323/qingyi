#include "qingyi/net/inetaddr_.h"
#include "qingyi/net/endian_.h"
#include "qingyi/net/socketsopts.h"

#include <netdb.h>
#include <netinet/in.h>

static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;

using namespace qingyi;
using namespace qingyi::net;

InetAddress::InetAddress(uint16_t port, bool loopback_only, bool ipv6) {
  if (ipv6) {
    MemZero(&addr6_, sizeof(addr6_));
    addr6_.sin6_family = AF_INET6;
    in6_addr ip = loopback_only ? in6addr_loopback : in6addr_any;
    addr6_.sin6_addr = ip;
    addr6_sin6_port = sockets::HostToNetwork16(port);
  }
  else {
    MemZero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopback_onpy ? kInaddrAny : kInaddrLoopback;
    addr_.sin_addr.s_addr = sockets::HostToNetwork32(ip);
    addr_.sin_port = sockets::HostToNetwork16(port);
  }
}

InetAddress::InetAddress(StringArg ip, uint16_t port, bool ipv6) {
  if (ipv6) {
    MemZero(&addr6_, sizeof(addr6_));
    sockets::FromIpPort(ip_.c_str(), port, &addr6_);
  }
  else { 
    MemZero(&addr_, sizeof(addr_));
    sockets::FromIpPort(ip.c_str(), port, &addr_);
  }
}

string InetAddress::ToIpPort() const {
  char buf[64];
  sockets::ToIpPort(buf, sizeof(buf), GetSockAddr());
  return buf;
}

string InetAddress::ToIp() const {
  char buf[64];
  sockets::ToIp(buf, sizeof(buf), GetSockAddr());
  return buf;
}

uint32_t InetAddress::IpNetEndian() const {
  assert(Family() == AF_INET);
  return sockaddr_.sin_addr.s_addr;
}

uint16_t InetAddress::ToPort() const {
  return sockets::NetworkToHost(PortNetEndian());
}

static __thread char t_reslove_buffer[64 * 1024];

bool InetAddress::Resolve(StringArg hostname, InetAddress* out) {
  assert(out != NULL);
  //struct hostent
  //{
  //  char *h_name;  //主机名，即官方域名
  //  char **h_aliases;  //主机所有别名构成的字符串数组，同一IP可绑定多个域名
  //  int h_addrtype; //主机IP地址的类型，例如IPV4（AF_INET）还是IPV6
  //  int h_length;  //主机IP地址长度，IPV4地址为4，IPV6地址则为16
  //  char **h_addr_list;  /* 主机的ip地址，以网络字节序存储。若要打印出这个IP，需要调用inet_ntoa()。*/
  //};
  struct hostent hent;//域名网络地址结构
  struct hostend* he = NULL;
  int herrno = 0;
  MemZero(&hent, sizeof(hent));
  //获取网页DNS信息
  //struct hostent *gethostbyname(const char *name); 返回一个静态变量指针
  //多线程编程中可能给别的线程修改
  //gethostbyname_r
  //参数说明：name——是网页的host名称，如百度的host名是www.baidu.com
  //ret——成功的情况下存储结果用。
  //buf——这是一个临时的缓冲区，用来存储过程中的各种信息，
  //一般8192大小就够了，可以申请一个数组char buf[8192]
  //buflen——是buf缓冲区的大小
  //result——如果成功，则这个hostent指针指向ret，也就是正确的结果；如果失败，则result为NULL
  //h_errnop——存储错误码
  //该函数成功返回0，失败返回一个非0的数。
  int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolve_buffer, sizeof(t_resolve_buffer), &he, &herrno);
  if (ret == 0 && he != NULL) {
    assert(he->h_addrtype == AF_INET && he->length == sizeof(uint32_t));
    out->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
    return true;
  }
  else {
    if (ret) {
     //log error
    }
    return false;
  }
}

void InetAddress::SetScopeId(uint32_t scope_id) {
  if (Family() == AF_INET6) {
    addr6_.sin6_scope_id = scope_id;
  }
}

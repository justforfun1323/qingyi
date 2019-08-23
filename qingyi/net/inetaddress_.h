#ifndef QINGYI_NET_INEADDRESS_H
#define QINGYI_NET_INEADDRESS_H

#include "qingyi/util/copyable_.h"
#include "qingyi/util/stringpiece_.h"
#include "qingyi/net/socketsopts_.h"

#include <netinet/in.h>

namespace qingyi{
namespace net{
namespace sockets{

//const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr); 

} //end of sockets

class InetAddress : public copyable {
 private:
   union {
     struct sockaddr_in addr_;
     struct sockaddr_in6 addr6_;
   };
 public:
   explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

   InetAddress (StringArg ip, uint16_t port, bool ipv6 = false);
  
   explicit InetAddress(const struct sockaddr_in& addr)
     : addr_(addr) { 
   }
 
   explicit InetAddress(const struct sockaddr_in6& addr)
     : addr6_(addr) {
   }

   sa_family_t Family() const {
     return addr_.sin_family;
   }
  
   string ToIp() const;

   string ToIpPort() const;

   uint16_t ToPort() const;
 
   const struct sockaddr* GetSockAddr() const {
     return sockets::sockaddr_cast(&addr6_);
   }

   void SetSockAddrInet6(const struct sockaddr_in6& addr6) {
     addr6_ = addr6;
   }

   uint32_t IpNetEndian() const;
 
   uint16_t PortNetEndian() const {
     return addr_.sin_port;
   }

   static bool Resolve(StringArg hostname, InetAddress* result);

   void SetScopeId(uint32_t socope_id);

};
}//end of net
}//end of qingyi
#endif//QINGYI_NET_INETADDRESS_H

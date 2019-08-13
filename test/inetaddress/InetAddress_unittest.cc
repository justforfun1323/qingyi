#include "qingyi/net/inetaddress_.h"
#include <iostream>


//#define BOOST_TEST_MODULE InetAddressTest
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <iostream>
using qingyi::string;
using qingyi::net::InetAddress;


BOOST_AUTO_TEST_CASE(testInetAddress)
{
  InetAddress addr0(1234);
  BOOST_CHECK_EQUAL(addr0.ToIp(), string("0.0.0.0"));
  BOOST_CHECK_EQUAL(addr0.ToIpPort(), string("0.0.0.0:1234"));
  BOOST_CHECK_EQUAL(addr0.ToPort(), 1234);

  InetAddress addr1(4321, true);
  BOOST_CHECK_EQUAL(addr1.ToIp(), string("127.0.0.1"));
  BOOST_CHECK_EQUAL(addr1.ToIpPort(), string("127.0.0.1:4321"));
  BOOST_CHECK_EQUAL(addr1.ToPort(), 4321);

  InetAddress addr2("1.2.3.4", 8888);
  BOOST_CHECK_EQUAL(addr2.ToIp(), string("1.2.3.4"));
  BOOST_CHECK_EQUAL(addr2.ToIpPort(), string("1.2.3.4:8888"));
  BOOST_CHECK_EQUAL(addr2.ToPort(), 8888);

  InetAddress addr3("255.254.253.252", 65535);
  BOOST_CHECK_EQUAL(addr3.ToIp(), string("255.254.253.252"));
  BOOST_CHECK_EQUAL(addr3.ToIpPort(), string("255.254.253.252:65535"));
  BOOST_CHECK_EQUAL(addr3.ToPort(), 65535);
}

BOOST_AUTO_TEST_CASE(testInetAddressResolve)
{
  InetAddress addr(80);
  if (InetAddress::Resolve("google.com", &addr))
  {
    std::cout << "google.com resolved to " << addr.ToIpPort() << std::endl;
  }
  else
  {
    std::cout << "Unable to resolve google.com" << std::endl;
  }
}

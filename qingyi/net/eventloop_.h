#ifndef QINGYI_NET_EVENTLOOP_H
#define QINGYI_NET_EVENTLOOP_H

#include "qingyi/util/mutex_.h"
#include "qingyi/util/current_thread_.h"
#include "qingyi/util/timestamp_.h"
#include "qingyi/net/callback_.h"
#include "qingyi/net/timerid_.h"


#include <atomic>
#include <functional>
#include <vector>

#include <boost/any.hpp>

namespace qingyi {

namespace net {

class Channel;
class Poller;
class TimerQueue;

class EventLoop : noncopyable {


};

}//namespace net

}//namespace qingyi


#endif//QINGYI_NET_EVENTLOOP_H

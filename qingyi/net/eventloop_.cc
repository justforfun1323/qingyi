#include "qingyi/net/eventloop_.h"

#include "qingyi/util/mutex_.h"
#include "qingyi/net/channel_.h"
#include "qingyi/net/poller_.h"
#include "qingyi/net/socketsopts_.h"
#include "qingyi/net/timer_queue.h"

#include <algorithm>

#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace qingyi;
using namespace qingyi::net;

namespace {

__thread EventLoop* t_loop_in_this_thread = 0;

const int kPollerTimeMs = 10000; //.01秒查询一次

int CreateEventFd() {
  int evtfd = ::eventfd(0, EFD|NONBLOCK | EFD_CLOEXEC);
  if (evefd < 0) {
    //log error
    abort();
  }
  return evtfd;
}

class IgnoreSigpipe {
  public:
    IgnoreSigpipe() {
      ::signal(SIGPIPE, SIG_IGN);
    }
};

IgnoreSigpipe initObj;

} //namesapce

EventLoop* EventLoop::GetEventLoopOfCurrentThread() {
  return t_loop_in_this_thread;
}

EventLoop::EventLoop()
  : looping_(false),
    quit_(false),
    event_handling_(false),
    calling_pending_functors_(false),
    iteration_(0),
    thread_id_(currentthread::Tid()),
    poller_(Poller::newDefaultPoller(this),
    timerqueue_(new TimerQueue(this)),
    wakeupfd_(CreateEventfd()),
    wakeup_channel_(new Channel(this, wakeupfd_)),
    current_active_channel_(NULL) {
  if (t_loop_in_this_thread) {
    //log
  }
  else {
    t_loop_in_this_thread = this;
  }
  wakeup_channel_->set_read_callback(std::bind(&EventLoop::HandleRead, this));
  wakeup_channel_->EnableReading();
}


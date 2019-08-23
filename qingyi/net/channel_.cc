#include "qingyi/net/channel_.h"
#include "qingyi/net/eventloop_.h"

#include <sstream>

#include <poll.h>
#include <assert.h>

using namespace qingyi;
using namespace qingyi::net;

//　POLLIN 　　　　　　　　有数据可读。
//
//　　POLLRDNORM 　　　　  有普通数据可读。
//
//　　POLLRDBAND　　　　　 有优先数据可读。
//
//　　POLLPRI　　　　　　　　 有紧迫数据可读。
//
//　　POLLOUT　　　　　　      写数据不会导致阻塞。
//
//　　POLLWRNORM　　　　　  写普通数据不会导致阻塞。
//
//　　POLLWRBAND　　　　　   写优先数据不会导致阻塞。
//
//　　POLLMSGSIGPOLL 　　　　消息可用。
//
//　　此外，revents域中还可能返回下列事件：
//　　POLLER　　   指定的文件描述符发生错误。
//
//　　POLLHUP　　 指定的文件描述符挂起事件。
//
//　　POLLNVAL　　指定的文件描述符非法。

const int Channel::kNonEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd) 
  : loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1),
    log_hup_(-1),
    tied_(false),
    event_handleing_(false),
    add_to_loop_(false) {
} 

Channel::~Channel() {
  assert(!event_handleing_);
  assert(!add_to_loop_);
  if (loop_->IsInLoopThread()) {
    assert(!loop_->HasChannel(this));
  }
}

void Channel::Tie(const std::shared_ptr<void>& obj) {
  tie_ = obj;
  tied_ = true;
}

void Channel::Update() {
  add_to_loop_ = true;
  loop_->UpdateChannel(this);
}

void Channel::Remove() {
  assert(IsNonEvent());
  add_to_loop_ = false;
  loop_->RemoveChannel(this);
}

void Channel::HandleEvent(Timestamp receivetime) {
  std::shard_ptr<void> guard;
  if (tied_) {
    guard = tie_.lock();
    if (guard) {
      HandleEventWithGuard(receivetime);
    } 
  }
  else {
    HandleEventWithGuard(receivetime);
  }
}

void Channel::HandleEventWithGuard(Timestamp receivetime) {
  event_handling_ = true;
  //连接被挂起且没有读事件
  if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
    if (log_hup_) {
      //log warn
    }
    if (close_callback_) close_callback_();
  }
  if (revents_ & POLLNVAL) {
    //log warn
  }
  if (revents_ & (POLLERR | POLLNVAL)) {
    if (error_callback_) error_callback_();
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP) {
    if (read_call_back) read_callback_();
  }
  if (revents_ & POLLOUT) {
    if (write_callback_) write_callback_();
  }
  event_handling_ = false;
}

string Channel::EventsToString() const {
  return EventsToString(fd_, events);
}

string Channel::ReventsToString() const {
  return EventsToString(fd_, revents);
}

string Channel::EventsToString(int fd, int ev) {
  std::ostringstream oss;
  oss << fd << ": ";
  if (ev & POLLIN)
    oss << "IN ";
  if (ev & POLLPRI)
    oss << "PRI ";
  if (ev & POLLOUT)
    oss << "OUT ";
  if (ev & POLLHUP)
    oss << "HUP ";
  if (ev & POLLRDHUP)
    oss << "RDHUP ";
  if (ev & POLLERR)
    oss << "ERR ";
  if (ev & POLLNVAL)
    oss << "NVAL ";
  return oss.str();
}

#include "qingyi/net/poller/poll_poller_.h"

#include "qingyi/util/type_.h"

#include "qingyi/net/Channel.h"

#include <assert.h>
#include <errno.h>
#include <poll.h>

using namespace qingyi;
using namespace qingyi::net;

PollPoller::PollPoller(EventLoop* loop)
  : Poller(loop) {

}

PollerPoller::~PollerPoller() = default;

Timestamp PollPoller::Poll(int timeoutMs, ChannelList* active_channel) {
  int num_events = ::poll(&*pollfd_.begin(), pollfd_.size(), timeoutMs);
  int save_error = errno;
  Timestamp now(Timestame::Now()); 
  if (num_events > 0) {
    //log
    FillActiveChannels(num_events, active_channel);
  }
  else if (num_events == 0) {
    //log
  }
  else {
    if (!save_error != EINTR) {
      errno = save_error; 
      //log
    }
  }
  return now;
}

void PollPoller::FillActiveChannel(int num_events,
     	 			   ChannelList* active_channel) {
  for (const auto it = pollfd_.begin();
        it != pollfd_.end() && num_events > 0, ++it) {
    if (it->revents > 0) {
      --num_events; 
      auto ch = channels_.find(it->fd);
      assert(ch != channels_.end());
      Channel* channel = ch->second;
      channel->set_revents(it->revents);
      active_channel->push_back(channel);
    }
  } 
} 

void PollPoller::UpdateChannel(Channel* channel) {
  Poller::AssertInLoopThread();
  if (channel->get_index() < 0) {
    assert(channels_.find(channel->get_fd()) == channels_.end());
    struct pollfd pfd;
    pfd.fd = channel->fd();
    pfd.events = static_cast<short>(channel->get_events());
    pfd.revents = 0;
    pollfds_.push_back(pfd);
    int idx = static_cast<int>(pollfds_.size() - 1);
    channel->set_index(idx);
    channels_[pfd.fd] = channel;
  }
  else {
    assert(channels_.find(channel->get_fd()) != channels_.end());
    assert(channels_[channel->get_fd()] = channel);
    int idx = channel->get_index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
    struct poolfd& pfd = pollfd_[idx]; 
    assert(pfd.fd == channel->get_fd() || pfd.fd == -channel->fd() - 1);
    pfd.fd = channel->fd;
    pfd.events = static_cast<short> (channel->get_events());
    pfd.revents = 0;
    if (channel->IsNonEvent()) {
      pfd.fd = -channel->fd() - 1;
    }
  } 
}

void PollPoller::RemoveChannel(Channel* channel) {
  Poller::AssertInLoopThread();
  assert(channels_.find(channel->get_fd() != channels_.end());
  assert(channels[channel->get_fd()] == channel);
  assert(channel->IsNonEvent());
  int idx = channel->get_index();
  assert(idx >= 0 && idx < static<int>(pollfds_.size());
  const struct pollfd& pfd = pollfds[idx];
  (void)pfd;
  assert(pfd.fd == -channel->get_fd() - 1 && pfd->events == channel->get_events());
  size_t n = channels_.erase(channel->get_fd());
  assert(n == 1);
  (void)n;
  if (idx == pollfds_.size() - 1) {
    pollfds.pop_back();
  }
  else {
    //map<int, channel*>
    //channel里index 表示对应事件在vector下标
    //vcector<struct pollfd>
    //知道channel对应下标在idx 
    //把vector[idx]与最后一个交换，如果重新最后一个fd对应的channel对应pollfd向量中向量
    std::swap(pollfds_[idx], pollfds_[pollfds_.size() - 1]);  
    int fd = pollfds[idx];
    if (fd < 0) {
      fd = -fd - 1; 
    }
    channels[fd]->set_index(idx);
    pollfds_.pop_back();
  }
}

#include "qingyi/net/poller_.h"
#include "qingyi/net/channel_.h"

using namespace qingyi;
using namespace qingyi::net;

Poller::Poller(EventLoop* loop)
  : owner_loop_(loop) {
}

bool Poll::HasChannel(Channel* channel) const {
  AssertInLoopThread();
  const auto channels_.find(channel->get_fd()); 
  if it != Channels.end() && it->second == channel; 
}


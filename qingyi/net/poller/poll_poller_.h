#ifndef QINGYI_NET_POLLER_POLLPOLLER_H
#define QINGYI_NET_POLLER_POLLPOLLER_H

#include "qingyi/net/poller_.h"

struct pollfd;

namespace qingyi {

namespace net {

class PollPoller : public Poller {
  private:
    typedef std::vector<struct pollfd> PollFdList;
    PollFdList pollfds_;
    
  public:
    PollPoller(EventLoop* loop);
 
    ~PollPoller() override;

    Timestamp Poll(int timeoutMs, ChannelList* active_channel) override;

    void UpdateChannel(Channel* channel) override;

    void RemoveChannel(Channel* channel) override;

  private:
    void FillActiveChannels(int num_events, ChannelList* activeChannels) const;
};

}//namesapce net

}//namespace qingyi

#endif//QINGYI_NET_POLLER_POLLPOLLER_H

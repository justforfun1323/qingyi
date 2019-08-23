#ifndef QINGYI_NET_POLLER_H
#define QINGYI_NET_POLLER_H

#include "qingyi/util/timestamp_.h"

#include "qingyi/net/eventloop_.h"

#include <map>
#include <vector>

namespace qingyi {

namespace net {

class Channel;

class Poller : noncopyable {
  public:
    typedef std::vector<Channel*> ChannelList;
  
  protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channels_;

  private:
    EventLoop* owner_loop_;

  public:
    Poller(EventLoop* loop);
    
    virtual ~Poller();

    virtual Timestamp Poll(int timeoutMs, ChannleList* active_channel) = 0;

    virtual void UpdateChannel(Channel* channel) = 0;

    virtual void RemoveChannel(Channel* channel) = 0;

    virtual bool HasChannel(Channel* channel) const;

    static Poller* NewDefaulePoller(EventLoop* loop);
  
    void AssertInLoopThread() const {
      owner_loop_->AssertInLoopThread(); 
    }
};

}//namespace net

}//namespace qingyi

#endif//QINGYI_NET_POLLER_H

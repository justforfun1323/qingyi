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
  public:
    typedef std::function<void()> Functor;

  private:
    typedef std::vector<Channel*> ChannelList;
    bool looping_;
    std::atomic<bool> quit_;
    bool event_handling_;
    bool calling_pending_functors_;  
    int64_t iteration_;
    const pid_t thread_id_;
    Timestamp poller_return_time_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerquque_;
    int wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channle_;
    boost::any context_;
    ChannelList active_channels_;
    Channel* current_active_channel_;
    mutable Mutex mutex_;
    std::vector<Functor> pending_functors_;
 
  public:
    EventLoop();

    ~EventLoop();

    void Loop();

    void Quit();

    Timestamp get_poller_return_time() const {
      return poller_return_time_; 
    }

    int64_t get_iteration() const {
      return iteration_; 
    }
  
    void RunInLoop(Functor cb); 

    void QueueInLoop(Fuctor cb);

    size_t GetQueueSize() const;
   
    TimerId RunAt(Timestamp time, TimerCallback cb);
  
    TimerId RunAfter(double interval, TimerCallback cb);

    TimerId RunEvery(double interval, TimerCallback cb);

    void Cancel(TimerId timerId);
 
    void WakeUp();
  
    void UpdateChannel(Channel* channel);

    void RemoveChannel(Channel* channel);

    bool HasChannel(Channel* channel);

    void AssertInLoopThread() {
      if (!IsInLoopThread() {
        AbortNotInLoopThread();
      }
    }

    bool IsInLoopThread() const {
      return thread_id_ == current_thread::Tid();
    }

    bool get_event_handling() const {
      return event_handling_;
    }

    void set_context(boost::any& context) {
      context_ = context;
    }

    const boost::any& get_context() const {
      return context_;
    }

    boost::any* get_mutable_context() const {
      return &context_;
    }

    static EventLoop* GetEventLoopOfCurrentThread();
};

}//namespace net

}//namespace qingyi


#endif//QINGYI_NET_EVENTLOOP_H

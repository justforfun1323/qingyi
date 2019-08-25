#ifndef QINGYI_NET_CHANNEL_H
#define QINGYI_NET_CHANNEL_H

#include "qingyi/util/noncopyable_.h"
#include "qingyi/util/timestamp_.h"

#include <functional>
#include <memory>

namespace qingyi {

namespace net {

class EventLoop;

class Channel : noncopyable {
  public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(Timestamp)> ReadEventCallback;
  private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
 
    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;
    bool log_hup_;

    std::weak_ptr<void> tie_;
    bool tied_;
    bool event_handling_;
    bool added_to_loop_;
    ReadEventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback close_callback_;
    EventCallback error_callback_;

  Channel(EventLoop* loop, int fd);

  ~Channel();

  void HandleEvent(Timestamp receive_time);

  void set_read_callback(ReadCallback cb) {
    read_callback_ = std::move(cb);
  }

  void set_write_callback(EventCallback cb) {
    write_call_back_ = std::move(cb);
  }

  void set_close_callback(EventCallback cb) {
    close_callback_ = std::move(cb);
  }

  void set_error_callback(EventCallback cb) {
    error_callback_ = std::move(cb);
  }

  void Tie(const std::shared_ptr<void>&);

  int get_fd() const {
    return fd_;
  }

  int get_events() const {
    return events_;
  }

  void set_revents(int revt) {
    revents_ = revt;
  }

  bool IsNoneEvent() const {
    return events_ == kNoneEvent;
  }

  void EnableReading() {
    events_ |= kReadEvent;
    Update();
  }

  void DisableReading() {
    events_ &= ~kReadEvent;
    Update();
  }

  void EnableWriting() {
    events_ |= kWriteEvent;
    Update();
  }

  void DisableWriting() {
    events_ &= ~kWriteEvent;
    Update();
  }

  void DisableAll() {
    events_ = kNoneEvent;
  }

  bool IsWriting() const {
    return events_ & kWriteEvent; 
  }

  bool IsReading() const {
    return events_ & kReadEvent;
  }

  int get_index() const {
    return index_;
  }

  void set_index(int idx) {
    index_ = idx; 
  }

  string ReventsToString() const;

  string EventsToString() const;

  void DoNotLogHup() {
    log_hup_ = false;
  }

  EventLoop* get_owner_loop() const {
    return loop_;
  }

  void Remove();

private:
  static stirng EventsToString(int fd, int ev);

  void Update();

  void HandleEventWithGuard(Timestamp receivetime);
};

}//namespace net

}//namespace qingyi

#endif//QINGYI_NET_CHANNEL_H

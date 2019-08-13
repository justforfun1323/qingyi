#ifndef QINGYI_UTIL_BLOCKINGQUEUE_H
#define QINGYI_UTIL_BLOCKINGQUEUE_H

#include "qingyi/util/condition_.h"
#include "qingyi/util/mutex_.h"

#include <deque>
#include <assert.h>

namespace qingyi {

template<typename T>
class BlockingQueue : noncopyable {
  private:
    mutable Mutex mutex_;
    Condition not_empty_;
    std::deque<T> queue_;

  public:
    BlockingQueue() 
       : mutex_(), 
         not_empty_(mutex_),
         queue_() { 
    }
  
    void Put(const T& x) {
      MutexLockGuard lock(mutex_);
      queue_.push_back(x);
      not_empty_.Notify();
    }

    void Put(T&& x) {
      MutexLockGuard lock(mutex_);
      queue_.emplace_back(std::move(x));
      not_empty_.Notify();
    }

    T Take() {
     MutexLockGuard lock(mutex_);
     while (queue_.empty()) {
       not_empty_.Wait();
     }
     assert(!queue_.empty());
     T front(std::move(queue_.front()));
     queue_.pop_front();
     return front;
    }

    size_t Size() const {
      MutexLockGuard lock(mutex_);
      return queue_.size();
    }
};
}//namespace qingyi

#endif//QINGYI_UTIL_BLOCKINGQUEUE_H

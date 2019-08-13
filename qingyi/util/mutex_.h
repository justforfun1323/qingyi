#ifndef QINGYI_BASE_MUTEX_H
#define QINGYI_BASE_MUTEX_H

#include "qingyi/util/current_thread_.h"
#include "qingyi/util/noncopyable_.h"

#include <assert.h>
#include <pthread.h>

namespace qingyi {
class Mutex : noncopyable {
  private:
    pthread_mutex_t mutex_;
    pid_t holder_;
  public:
    Mutex() : holder_(0) {
      pthread_mutex_init(&mutex_, 0);
    }

    ~Mutex() {
      assert(holder_ == 0);
      pthread_mutex_destroy(&mutex_);
    }

    bool IsLockedByThisThread() const {
      return holder_ == currentthread::Tid(); 
    }
  
    void AssertLocked() const {
      assert(IsLockedByThisThread());
    }

    void Lock() {
      pthread_mutex_lock(&mutex_);   //可能有多个线程同时拿锁，所以先拿到锁后，在设定pid
      AssignHolder();
    }
   
    void UnLock() {
      UnAssignHolder();  //如果先释放锁，可能会出现，释放锁后，pid_t 还是这个线
      pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* get_pthread_mutex() {
      return &mutex_;
    }

  private:
    friend class Condition;
   
    class UnGuard : noncopyable {   //这个内部类实现了，构造释放锁线程ID， 析构设置锁线程ID
      private:
        Mutex& owner_;
      public:
        explicit UnGuard(Mutex& owner) : owner_(owner) {
          owner_.UnAssignHolder();
        }
       
        ~UnGuard() {
          owner_.AssignHolder(); 
        }
    };   
    
    void UnAssignHolder() {
      holder_ = 0;
    }

    void AssignHolder() {
      holder_ = currentthread::Tid();
    }
};

class MutexLockGuard : noncopyable {
  private:
    Mutex& mutex_;

  public:
    explicit MutexLockGuard(Mutex& mutex) : mutex_(mutex) {
      mutex_.Lock();
    }
   
    ~MutexLockGuard() {
      mutex_.UnLock();
    }
};


}//namespace qingyi
#endif //QINGYI_BASE_MUTEX_H

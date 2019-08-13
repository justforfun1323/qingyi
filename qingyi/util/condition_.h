#ifndef QINGYI_BASE_CONDITION_H
#define QINGYI_BASE_CONDITION_H

#include "qingyi/util/mutex_.h"

#include <pthread.h>

namespace qingyi {
class Condition : noncopyable {
  private:
    Mutex& mutex_;
    pthread_cond_t pcond_;

  public: 
    explicit Condition(Mutex& mutex) : mutex_(mutex) {
      pthread_cond_init(&pcond_, NULL);
    }
  
    ~Condition() {
      pthread_cond_destroy(&pcond_); 
    }

    void Wait() {
      Mutex::UnGuard ug(mutex_);  //在Wait时候释放锁的holder_
      pthread_cond_wait(&pcond_, mutex_.get_pthread_mutex());
    }

    bool WaitForSeconds(double seconds);

    void Notify() {
      pthread_cond_signal(&pcond_); 
    }

    void NotifyAll() {
      pthread_cond_broadcast(&pcond_);
    }
};
}//namespace qingyi

#endif // QINGYI_BASE_CONDITION_H

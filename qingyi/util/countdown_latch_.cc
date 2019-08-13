#include "qingyi/util/countdown_latch_.h"

using namespace qingyi;

CountDownLatch::CountDownLatch(int count) 
 : mutex_(),
   condition_(mutex_), //注意条件变量的锁和mutex_是同一把锁
   count_(count) {
  
}

void CountDownLatch::Wait() {
  MutexLockGuard lock(mutex_);
  while (count_ > 0) {
    condition_.Wait();
  }
}

void CountDownLatch::CountDown() {
  MutexLockGuard lock(mutex_);
  --count_;
  if (count_ == 0) {
    condition_.NotifyAll();
  }
}

int CountDownLatch::get_count() const  {
  MutexLockGuard lock(mutex_);
  return count_;
}

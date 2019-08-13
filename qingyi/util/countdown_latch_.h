#ifndef QINGYI_BASE_COUNTDOWNLATCH_H
#define QINGYI_BASE_COUNTDOWNLATCH_H

#include "qingyi/util/condition_.h"
#include "qingyi/util/mutex_.h"

namespace qingyi {
class CountDownLatch : noncopyable {
  public:
    explicit CountDownLatch(int count);

    void Wait();
  
    void CountDown();

    int get_count() const ;

  private:
    mutable Mutex mutex_; 
    Condition condition_;
    int count_;
};
}//namespace qingyi
#endif //QINGYI_BASE_COUNTDOWNLATCH_H

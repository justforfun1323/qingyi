#ifndef QINGYI_BASE_THREAD_H
#define QINGYI_BASE_THREAD_H

#include "qingyi/util/atomicint_.h"
#include "qingyi/util/countdown_latch_.h"
#include "qingyi/util/type_.h"

#include <functional>
#include <memory>
#include <pthread.h>

namespace qingyi {
class Thread : noncopyable {
  public: 
    typedef std::function<void()> ThreadFunc;

  private:
    bool started_;
    bool joined_;
    pthread_t pthread_id_;
    pid_t tid_;
    ThreadFunc func_;
    string name_;
    CountDownLatch latch_;
    static AtomicInt32 num_created_;  

  public:
    explicit Thread(ThreadFunc , const string& name = string());
     
    ~Thread();

    void Start();  
 
    int Join();

    bool Started() const {
      return started_;
    }

    pid_t Tid() {
      return tid_;
    }

    const string& Name() const {
      return name_;
    }

    static int NumCreated() {
      return num_created_.Get();
    }
  
  private:
    void set_default_name();
};
} // namesapce qingyi

#endif // QINGYI_BASE_THREAD_H

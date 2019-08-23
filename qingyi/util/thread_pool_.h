#ifndef QINGYI_UTIL_THREADPOOL_H
#define QINGYI_UTIL_THREADPOOL_H

#include "qingyi/util/condition_.h"
#include "qingyi/util/mutex_.h"
#include "qingyi/util/type_.h"
#include "qingyi/util/thread_.h"

#include <deque>
#include <vector>
#include <functional>
#include <memory>

namespace qingyi {
class ThreadPool : noncopyable {
  public:
   typedef std::function<void () > Task;

  private:
    mutable Mutex mutex_;
    Condition not_empty_;
    Condition not_full_;
    string name_;
    Task thread_init_callback_;
    std::vector<std::unique_ptr<qingyi::Thread>> threads_;
    std::deque<Task> queue_;
    size_t max_queue_size_;
    bool running_;

  public:
    explicit ThreadPool(const string& name = string("ThreadPool"));
      
    ~ThreadPool();

    void set_max_queue_size(int max_queue_size) {
      max_queue_size_ = max_queue_size;
    }
  
    void set_thread_init_callback(const Task& cb) {
      thread_init_callback_ = cb;
    }

    void Start(int num_threads);
 
    void Stop();
    
    const string& name() const {
      return name_;
    }

    size_t quque_size() const;
      
    void Run(Task t);

  private:
    bool IsFull() const;
    
    void RunInThread();

    Task Take();
};

}//namespace qingyi


#endif//QINGYI_UTIL_THREADPOOL_H

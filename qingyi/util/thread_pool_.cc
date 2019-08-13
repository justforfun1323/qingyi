#include "qingyi/util/thread_pool_.h"

#include <assert.h>
#include <stdio.h>

using namespace qingyi;

ThreadPool::ThreadPool(const string& name)
	: mutex_(),
  	  not_empty_(mutex_),
	  not_full_(mutex_),
	  name_(name),
 	  max_queue_size_(0),
	  running_(false) {
}

ThreadPool::~ThreadPool() {
  if (running_) {
    Stop();
  }
}

void ThreadPool::Start(int num_threads) {
  assert(threads_.empty());
  running_ = true;
  threads_.reserve(num_threads);
  for (int i = 0; i < num_threads; i++) {
    char id[32];
    snprintf(id, sizeof(id), "%d", i + 1);
    threads_.emplace_back(new qingyi::Thread(std::bind(&ThreadPool::RunInThread, this),
                       name_ + std::string(id)));
    threads_[i]->Start();
  }
   
  if (num_threads == 0 && thread_init_callback_) {
    thread_init_callback_();
  }
}

void ThreadPool::Stop() {
  {
  MutexLockGuard lock(mutex_);
  running_ = false;
  not_empty_.NotifyAll();
  }
  for (auto& thread : threads_) {
    thread->Join();
  }
}

size_t ThreadPool::quque_size() const {
  MutexLockGuard lock(mutex_);
  return queue_.size();
}

void ThreadPool::Run(Task t) {
  if (threads_.empty()) {
    t(); 
  }
  else {
    MutexLockGuard lock(mutex_);
    while (IsFull()) {
      not_full_.Wait();
    }
    assert(!IsFull());
    queue_.push_back(std::move(t));
    not_empty_.Notify();
  }
} 

ThreadPool::Task ThreadPool::Take() {
  MutexLockGuard lock(mutex_);
  while (queue_.empty() && running_) {
    not_empty_.Wait();
  }
  Task task;
  if (!queue_.empty()) {
    task = queue_.front();
    queue_.pop_front();
    if (max_queue_size_ > 0) {
      not_full_.Notify();
    }
  }
  return task;
}

bool ThreadPool::IsFull() const {
  mutex_.AssertLocked(); 
  return max_queue_size_ > 0 && queue_.size() >= max_queue_size_;
}

void ThreadPool::RunInThread() {
  if (thread_init_callback_) {
   thread_init_callback_();
  }
  while (running_) {
    Task task(Take());
    if (task) {
      task();
    }
  }
}

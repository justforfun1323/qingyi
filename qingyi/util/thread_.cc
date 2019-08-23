#include "qingyi/util/thread_.h"
#include "qingyi/util/current_thread_.h"
#include "qingyi/util/timestamp_.h"

#include <type_traits>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/unistd.h>

namespace qingyi {
namespace detail {

pid_t GetTid() { //得到内核id, 个人理解，不一定正确
  return static_cast<pid_t> (::syscall(SYS_gettid));
} 

void AfterFork() {
  qingyi::currentthread::t_cached_tid = 0;
  qingyi::currentthread::t_thread_name = "main";
  currentthread::Tid();
}

class ThreadNameInitializer {
  public:
    ThreadNameInitializer() {
      qingyi::currentthread::t_thread_name = "main";
      currentthread::Tid();
      //pthread_atfork(void (*prepare)(void），void (*parent)(void）, void(*child)(void))
      //prepare在父进程fork创建子进程之前调用，这里可以获取父进程定义的所有锁；
      //child fork返回之前在子进程环境中调用，在这里unlock prepare获得的锁；
      //parent fork创建了子进程以后，但在fork返回之前在父进程的进程环境中调用的，在这里对prepare获得的锁进行解锁；
      pthread_atfork(NULL, NULL, &AfterFork);
    }
};

ThreadNameInitializer init;

struct ThreadData {
  typedef qingyi::Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;
  string name_; 
  pid_t* tid_;
  CountDownLatch* latch_;
  
  ThreadData(ThreadFunc func,
             const string& name,
             pid_t* tid,
             CountDownLatch* latch)
  : func_(std::move(func)),
    name_(name),
    tid_(tid),
    latch_(latch)
  { }
  
  void RunInThread() {
    *tid_ = qingyi::currentthread::Tid(); //设置内核线程编号
    tid_ = NULL;
    latch_->CountDown();
    latch_ = NULL;
    qingyi::currentthread::t_thread_name = name_.empty() ? "qingyiThread" : name_.c_str();
//int prctl ( int option,unsigned long arg2,unsigned long arg3,unsigned long arg4,unsigned long arg5 )
//
//这个系统调用指令是为进程制定而设计的，明确的选择取决于option:
//PR_SET_NAME :把参数arg2作为调用进程的经常名字。（SinceLinux 2.6.11）
    ::prctl(PR_SET_NAME, qingyi::currentthread::t_thread_name);
    func_();
    qingyi::currentthread::t_thread_name = "finished";
  }

};

  void* StartThread(void* obj) {
    ThreadData* data = static_cast<ThreadData*> (obj);
    data->RunInThread();
    delete data;
    return NULL;
  }

} //namespace detail

void currentthread::CachedTid() {
  if (t_cached_tid == 0) {
    t_cached_tid = detail::GetTid();
    t_tid_string_length = snprintf(t_tid_string, sizeof(t_tid_string), "%5d", t_cached_tid);
  }
}

bool currentthread::IsMainThread() {
   //Tid() 返回线程变量t_cached_id 
   return Tid() == ::getpid();
} 

void currentthread::SleepUsec(int64_t usec) {
  struct timespec ts = { 0, 0 };
  ts.tv_sec = static_cast<time_t>(usec / TimeStamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(usec % TimeStamp::kMicroSecondsPerSecond * 1000);
//#include <time.h>
//int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);
//nanosleep()函数会导致当前的线程将暂停执行,直到rqtp参数所指定的时间间隔。或者在指定时间间隔
//内有信号传递到当前线程，将引起当前线程调用信号捕获函数或终止该线程。
//暂停时间可能超过请求时间,因为参数值是sleep粒度的整数倍数或者因为其他活动的系统调度。
//如果rmtp参数不为空，它所引用的timespec结构更新为包含剩余时间的间隔量（请求的时间减去实际睡眠时间）。
//如果rmtp参数为NULL，不返回的剩余时间。
  ::nanosleep(&ts, NULL);
}

AtomicInt32 Thread::num_created_; //定义thread的全局变量

Thread::Thread(ThreadFunc  func, const string& name)
        : started_(false),
          joined_(false),
          pthread_id_(0),
          tid_(0),
          func_(std::move(func)),
          name_(name),
          latch_(1) {
  set_default_name();
}

Thread::~Thread() {
  if (started_ && !joined_) {
    pthread_detach(pthread_id_); //让线程分离
  }
}

void Thread::set_default_name() {
  int num = num_created_.IncrementAndGet(); 
  if (name_.empty()) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Thread %d", num); //几号线程
    name_ = buf;
  }
}

void Thread::Start() {
  assert(!started_);
  started_ = true;
  detail::ThreadData* data = new detail::ThreadData(func_, name_, &tid_, &latch_);
  if (pthread_create(&pthread_id_, NULL, &detail::StartThread, data)) {
    started_ = false;
    delete data;
    //log error
  }
  else {
    latch_.Wait();
    assert(tid_ > 0);
  }
}

int Thread::Join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthread_id_, NULL);
}
} //namesapce qingyi

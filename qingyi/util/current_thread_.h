#ifndef QINGYI_BASE_CURRENTTHREAD_H
#define QINGYI_BASE_CURRENTTHREAD_H

#include "qingyi/util/type_.h"

namespace qingyi {
namespace currentthread {
  extern __thread int t_cached_tid; //所有的线程变量以t_开头
  extern __thread char t_tid_string[32];
  extern __thread int t_tid_string_length;
  extern __thread const char* t_thread_name;
  void CachedTid();

  inline int Tid() {
    if ( t_cached_tid == 0) {
      CachedTid();
    }
    return t_cached_tid;
  }

  inline const char* TidString() {
    return t_tid_string; 
  }

  inline int TidStringLength() {
    return t_tid_string_length;
  }

  inline const char* Name() {
    return t_thread_name;
  }

  bool IsMainThread();

  void SleepUsec(int64_t secs);

  string StackTrace(bool demangle); //不知道这个函数干嘛的，反正很牛逼的样子
}// namespace currentthread
} // namespace qingyi
#endif  //QINGYI_BASE_CURRENTTHREAD_H

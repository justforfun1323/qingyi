#include "qingyi/util/condition_.h"

#include <errno.h>

bool qingyi::Condition::WaitForSeconds(double sec) {
  struct timespec abstime;
 //int clock_gettime(clockid_t clk_id, struct timespect *tp);  
 //clockid_t clk_id 用于指定计时时钟的类型，有以下4种：
 //CLOCK_REALTIME:系统实时时间,随系统实时时间改变而改变,即
 //从UTC1970-1-1 0:0:0开始计时,中间时刻如果系统时间被用户该成其他,则对应的时间相应改变  
 //CLOCK_MONOTONIC:从系统启动这一刻起开始计时,不受系统时间被用户改变的影响  
 //CLOCK_PROCESS_CPUTIME_ID:本进程到当前代码系统CPU花费的时间  
 //CLOCK_THREAD_CPUTIME_ID:本线程到当前代码系统CPU花费的时间  
 //   struct timespec  {
 //   time_t tv_sec; /* seconds */ 
 //   long tv_nsec; /* nanoseconds */
 //   }
  clock_gettime(CLOCK_REALTIME, &abstime);

  const int64_t kNanoSecondsPerSecond = 1000000000;
  int64_t nanoseconds = static_cast<int64_t> (sec * kNanoSecondsPerSecond);
  
  abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond); //计算秒的间隔
  abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond); //计算纳秒剩余
  
  Mutex::UnGuard ug(mutex_);
  //pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t *mutex, const struct timespec * abstime)
  //当在指定时间内有信号传过来时，pthread_cond_timedwait()返回0，否则返回一个非0数（我没有找到返回值的定义);
  return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.get_pthread_mutex(), &abstime);  
}

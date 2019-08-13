#include "qingyi/util/thread_pool_.h"
#include "qingyi/util/countdown_latch_.h"
#include "qingyi/util/current_thread_.h"

#include <stdio.h>
#include <unistd.h>  // usleep

void print()
{
  printf("\ntid=%d\n", qingyi::currentthread::Tid());
}

void printString(const std::string& str)
{
  printf("\n tid=%d  sleep\n", qingyi::currentthread::Tid());
  usleep(100*1000);
}

void test(int maxSize)
{
  //LOG_WARN << "Test ThreadPool with max queue size = " << maxSize;
  printf("Test ThreadPool with max queue size = %d", maxSize); 
  printf("\n main tid = %d\n", qingyi::currentthread::Tid());
  qingyi::ThreadPool pool("MainThreadPool");
  pool.set_max_queue_size(maxSize);
  pool.Start(5);

 // LOG_WARN << "Adding";
  pool.Run(print);
  pool.Run(print);
  for (int i = 0; i < 100; ++i)
  {
    char buf[32];
    snprintf(buf, sizeof buf, "task %d", i);
    pool.Run(std::bind(printString, std::string(buf)));
  }
  //LOG_WARN << "Done";

  qingyi::CountDownLatch latch(1);
  pool.Run(std::bind(&qingyi::CountDownLatch::CountDown, &latch));
  latch.Wait();
  pool.Stop();
  
  printf("Done\n");
}

/*
 * Wish we could do this in the future.
void testMove()
{
  muduo::ThreadPool pool;
  pool.start(2);

  std::unique_ptr<int> x(new int(42));
  pool.run([y = std::move(x)]{ printf("%d: %d\n", muduo::CurrentThread::tid(), *y); });
  pool.stop();
}
*/

int main()
{ 
  printf("\nmain tid: %d\n", qingyi::currentthread::Tid());
  test(0);
  test(1);
  test(5);
  test(10);
  test(50);
}

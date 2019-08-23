#include "qingyi/util/bounded_blocking_queue_.h"
#include "qingyi/util/countdown_latch_.h"
#include "qingyi/util/thread_.h"

#include <string>
#include <vector>

#include <stdio.h>
#include <unistd.h>

class Test
{
 public:
  Test(int numThreads)
    : queue_(20),
      latch_(numThreads)
  {
    threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i)
    {
      char name[32];
      snprintf(name, sizeof name, "work thread %d", i);
      threads_.emplace_back(new qingyi::Thread(
            std::bind(&Test::threadFunc, this), qingyi::string(name)));
    }
    for (auto& thr : threads_)
    {
      thr->Start();
    }
  }

  void run(int times)
  {
    printf("waiting for count down latch\n");
    latch_.Wait();
    printf("all threads started\n");
    for (int i = 0; i < times; ++i)
    {
      char buf[32];
      snprintf(buf, sizeof buf, "hello %d", i);
      queue_.Put(buf);
      printf("tid=%d, put data = %s, size = %zd\n", qingyi::currentthread::Tid(), buf, queue_.Size());
    }
  }

  void joinAll()
  {
    for (size_t i = 0; i < threads_.size(); ++i)
    {
      queue_.Put("stop");
    }

    for (auto& thr : threads_)
    {
      thr->Join();
    }
  }

 private:

  void threadFunc()
  {
    printf("tid=%d, %s started\n",
           qingyi::currentthread::Tid(),
           qingyi::currentthread::Name());

    latch_.CountDown();
    bool running = true;
    while (running)
    {
      std::string d(queue_.Take());
      printf("tid=%d, get data = %s, size = %zd\n", qingyi::currentthread::Tid(), d.c_str(), queue_.Size());
      running = (d != "stop");
    }

    printf("tid=%d, %s stopped\n",
           qingyi::currentthread::Tid(),
           qingyi::currentthread::Name());
  } 
  qingyi::BoundedBlockingQueue<std::string> queue_;
  qingyi::CountDownLatch latch_;
  std::vector<std::unique_ptr<qingyi::Thread>> threads_;
};

void testMove()
{
#if BOOST_VERSION >= 105500L
  qingyi::BoundedBlockingQueue<std::unique_ptr<int>> queue(10);
  queue.put(std::unique_ptr<int>(new int(42)));
  std::unique_ptr<int> x = queue.Take();
  printf("took %d\n", *x);
  *x = 123;
  queue.put(std::move(x));
  std::unique_ptr<int> y;
  y = queue.take();
  printf("took %d\n", *y);
#endif
}

int main()
{
  printf("pid=%d, tid=%d\n", ::getpid(), qingyi::currentthread::Tid());
  testMove();
  Test t(5);
  t.run(100);
  t.joinAll();

  printf("number of created threads %d\n", qingyi::Thread::NumCreated());
}

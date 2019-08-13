#include "qingyi/util/singleton_.h"
#include "qingyi/util/current_thread_.h"
#include "qingyi/util/thread_.h"

#include <stdio.h>

class Test : qingyi::noncopyable
{
 public:
  Test()
  {
    printf("tid=%d, constructing %p\n", qingyi::currentthread::Tid(), this);
  }

  ~Test()
  {
    printf("tid=%d, destructing %p %s\n", qingyi::currentthread::Tid(), this, name_.c_str());
  }

  const qingyi::string& name() const { return name_; }
  void setName(const qingyi::string& n) { name_ = n; }

 private:
  qingyi::string name_;
};

class TestNoDestroy : qingyi::noncopyable
{
 public:
  // Tag member for Singleton<T>
  void no_destroy();

  TestNoDestroy()
  {
    printf("tid=%d, constructing TestNoDestroy %p\n", qingyi::currentthread::Tid(), this);
  }

  ~TestNoDestroy()
  {
    printf("tid=%d, destructing TestNoDestroy %p\n", qingyi::currentthread::Tid(), this);
  }
};

void threadFunc()
{
  printf("tid=%d, %p name=%s\n",
         qingyi::currentthread::Tid(),
         &qingyi::Singleton<Test>::GetInstance(),
         qingyi::Singleton<Test>::GetInstance().name().c_str());
  qingyi::Singleton<Test>::GetInstance().setName("only one, changed");
}

int main()
{
  qingyi::Singleton<Test>::GetInstance().setName("only one");
  qingyi::Thread t1(threadFunc);
  t1.Start();
  t1.Join();
  printf("tid=%d, %p name=%s\n",
         qingyi::currentthread::Tid(),
         &qingyi::Singleton<Test>::GetInstance(),
         qingyi::Singleton<Test>::GetInstance().name().c_str());
  qingyi::Singleton<TestNoDestroy>::GetInstance();
  printf("with valgrind, you should see %zd-byte memory leak.\n", sizeof(TestNoDestroy));
}

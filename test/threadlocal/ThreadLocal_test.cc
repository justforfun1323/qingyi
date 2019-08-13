#include "qingyi/util/threadlocal_.h"
#include "qingyi/util/current_thread_.h"
#include "qingyi/util/thread_.h"
#include <iostream>

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

qingyi::ThreadLocal<Test> testObj1;
qingyi::ThreadLocal<Test> testObj2;

void print()
{
  printf("tid=%d, obj1 %p name=%s\n",
         qingyi::currentthread::Tid(),
         &testObj1.Value(),
         testObj1.Value().name().c_str());
  printf("tid=%d, obj2 %p name=%s\n",
         qingyi::currentthread::Tid(),
         &testObj2.Value(),
         testObj2.Value().name().c_str());
}

void threadFunc()
{
  print();
  testObj1.Value().setName("changed 1");
  testObj2.Value().setName("changed 42");
  print();
}

int main()
{
  std::cout << "-----------------------" << std::endl;
  testObj1.Value().setName("main one");
  print();
  qingyi::Thread t1(threadFunc);
  t1.Start();
  t1.Join();
  testObj2.Value().setName("main two");
  print();

  pthread_exit(0);
}

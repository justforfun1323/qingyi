#include <iostream>
#include <thread>

#include "qingyi/util/atomicint_.h"

void thread_in(qingyi::AtomicInt32& a, int& b) {
  for (int i = 1; i <= 100000; i++) {
    a.Increment();
    b++;
  }
}

int main(void) {
  qingyi::AtomicInt32 a;
  int b = 0;
  std::thread thread1(thread_in, std::ref(a), std::ref(b));
  for (int i = 1; i <= 100000; i++) {
    a.Increment();
    b++;
  }
  thread1.join();
  std::cout << a.Get() << std::endl;
  std::cout << b << std::endl;
  return 0;
}

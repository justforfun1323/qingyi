#include <iostream>
#include <thread>

#include "qingyi/util/thread_.h"

qingyi::Mutex mutex;

void thread1_in(int& a) {
  for (int i = 1; i <= 100000; i++) {
    a++;
  } 
}



int main(void) {
  int a = 0;
  qingyi::Thread thread1(std::bind(thread1_in, std::ref(a)));
  for (int i = 1; i <= 100000; i++) {
    a++;
  }
  thread1.Start();
  thread1.Join();
  std::cout << a << std::endl;
  return 0;
}

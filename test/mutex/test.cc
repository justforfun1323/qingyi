#include <iostream>
#include <thread>

#include "qingyi/util/mutex_.h"

qingyi::Mutex mutex;

void thread1_in(int& a) {
  for (int i = 1; i <= 100000; i++) {
    a++;
  } 
}

void thread2_in(int& b) {
  for (int i = 1; i <= 100000; i++) {
    qingyi::MutexLockGuard lock(mutex);
    b++; 
  }
}

int main(void) {
  int a = 0, b = 0;
  std::thread thread1(thread1_in, std::ref(a));
  for (int i = 1; i <= 100000; i++) {
    a++;
  }
  thread1.join();
  std::cout << a << std::endl;

  std::thread thread2(thread2_in, std::ref(b));
  for (int i = 1; i <= 100000; i++) {
    qingyi::MutexLockGuard lock(mutex);
    b++;
  }
  thread2.join();
  std::cout << b << std::endl;
  return 0;
}

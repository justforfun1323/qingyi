#ifndef QINGYI_BASE_ATOMICINT_H
#define QINGYI_BASE_ATOMICINT_H

#include "qingyi/util/noncopyable_.h"

#include <stdint.h>

namespace qingyi {
namespace detail {
template<typename T>
class AtomicInt : noncopyable {
  private:
    //容易变化的   
    // int a = f(n) //函数调用               
    // mov eax dword ptr[esp + 8] //函数返回时，把一些值保存在寄存器中
    // b = a + 1                             
    // lea ecx, [eax + 1]  //没有volatile修饰,可能会使用直接从寄存器中取a的值 
    // b = a + 1;这条语句，对应的汇编指令是：lea ecx, [eax + 1]。
    // 由于变量a，在前一条语句a = fn(c)执行时，被缓存在了寄存器eax中，
    // 因此b = a + 1；语句，可以直接使用仍旧在寄存器eax中的a，
    // 来进行计算，对应的也就是汇编：[eax + 1]。
    // volatile int a = f(n)  
    // mov ecx dword ptr[esp + 8]
    // mov dword ptr[esp + OCH], ecx   //会对a进行写会内存
    // b = a + 1 
    // mov eax, dword ptr[esp + OCH]
    // inc eax...
    // 计算b = a + 1 时，会从内存中把a的值读出来
    // 从以上的两个用例，就可以看出C/C++ Volatile关键词的第一个特性：
    // 易变性。所谓的易变性，在汇编层面反映出来，就是两条语句，下一条语句不会直
    // 接使用上一条语句对应的volatile变量的寄存器内容，而是重新从内存中读取。
    // volatile的这个特性，相信也是大部分朋友所了解的特性
    // 不可优化的
    // 无用的变量可能被编译器优化掉，但是volatile不会优化掉
    // int a = 3, b = 4;
    // printf ("%d %d", a, b) 
    // 编译器优化后可能直接为printf("3 4");
    // 顺序性
    // 组织编译器对一些造作进行乱序优化
    volatile T value_;

  public:
    AtomicInt() : value_(0) { }
  
    T Get() {
      return __sync_val_compare_and_swap(&value_, 0, 0); //gcc编译器提高的方法，实现原子操作
    }

    T GetAndAdd(T x) {
      return __sync_fetch_and_add(&value_, x);
    }

    T AddAndGet(T x) {
      return GetAndAdd(x) + x;
    }

    T IncrementAndGet() {
      return AddAndGet(1);
    }

    T DecrementAndGet() {
      return AddAndGet(-1);
    }

    void Add(T x) {
      GetAndAdd(x);
    }

    void Increment() {
      IncrementAndGet();
    }

    void Decrement() {
      DecrementAndGet();
    }
  
    T GetAndSet(T new_value) {
      return __sync_lock_test_and_set(&value_, new_value);
    }
};
}//namesapce detail

using AtomicInt32 = detail::AtomicInt<int32_t>;
using AtomicInt64 = detail::AtomicInt<int64_t>;

}//namespace qingyi

#endif //QINGYI_BASE_ATOMICINT_H

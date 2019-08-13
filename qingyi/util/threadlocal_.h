#ifndef QINGYI_UTIL_THREADLOCAL_H
#define QINGYI_UTIL_THREADLOCAL_H

#include "qingyi/util/mutex_.h"
#include "qingyi/util/noncopyable_.h"

#include <pthread.h>

namespace qingyi {
//线程中特有的线程存储， Thread Specific Data.
//表面上看起来这是一个全局变量，所有线程都可以使用它
//而它的值在每一个线程中又是单独存储的。这就是线程存储的意义
//调用pthread_key_create()来创建该变量。该函数有两个参数，第一个参数就是上面声
//明的pthread_key_t变量，第二个参数是一个清理函数，用来在线程释放该线程存储的
//时候被调用。该函数指针可以设成 NULL，这样系统将调用默认的清理函数
//当线程中需要存储特殊值的时候，可以调用 pthread_setspcific() 。该函数有两个参
//数，第一个为前面声明的pthread_key_t变量，第二个为void*变量，这样你可以存储
//任何类型的值
//如果需要取出所存储的值，调用pthread_getspecific()。该函数的参数为前面提到的
//pthread_key_t变量，该函数返回void *类型的值。下面是前面提到的函数的原型
//int pthread_setspecific(pthread_key_t key, const void *value);
//void *pthread_getspecific(pthread_key_t key);
//int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
template <typename T>
class ThreadLocal : noncopyable {
  private:
    pthread_key_t pkey_;

  public:
    ThreadLocal() {
      pthread_key_create(&pkey_, &ThreadLocal::destructor);
    }
   
    ~ThreadLocal() {
      pthread_key_delete(pkey_);
    }

    T& Value() {
      T* thread_value = static_cast<T*>(pthread_getspecific(pkey_));
      if (!thread_value) {
        T* new_obj = new T();
        pthread_setspecific(pkey_, new_obj);
        thread_value = new_obj;
      }
      return *thread_value;
    }
  private:
    static void destructor(void* x) {
      T* obj = static_cast<T*>(x);
      typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
      T_must_be_complete_type dumy;
      delete obj;
    } 
};
}//namespace qingyi

#endif//QINGYI_UTIL_THREADLOCAL_H

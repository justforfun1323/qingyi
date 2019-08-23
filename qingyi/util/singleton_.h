#ifndef QINGYI_UTIL_SINGLETON_H
#define QINGYI_UTIL_SINGLETON_H

#include "qingyi/util/noncopyable_.h"

#include <assert.h>
#include <pthread.h>
#include <stdlib.h> //atexit

namespace qingyi  {
namespace detail{
 //SFINAE 技术
 //SFINAE 技术，即匹配失败不是错误，英文
 //Substitution Failure Is Not An Error，
 //其作用是当我们在进行模板特化的时候，会去选择那个正确的模板，避免失败
 //long multiply(int i, int j) { return i * j; }
 //
 //template <class T>
 //typename T::multiplication_result multiply(T t1, T t2)
 //{
 //    return t1 * t2;
 //    }
 //    int main(void)
 //    {
 //        multiply(4,5);
 //    }
 //    当我们编译的时候，会去匹配模板 multiply，但是由于我们不知道
 //    multiplication_result，根据 Substitution Failure Is Not An Error ，
 //    于是我们就去选择函数 multiply
 //    这种技术在代码中的一个大的用途就是在编译时期来确定某个 type 是否具有我们需要的性质，看代码
template<typename T>
struct has_no_destroy{
  template <typename C> static char test(decltype(&C::no_destroy)); //静态函数
  template <typename C> static int32_t test(...);  //模板类里套用模板函数 
                   //如果C里没有no_destory调用这个函数，返回int32_t
  const static bool value = sizeof(test<T>(0)) == 1;
}; 
}//namespace detail

template<typename T>
class Singleton : noncopyable {
  private:
    static pthread_once_t ponce_;
    static T*             value_;

  public:
    Singleton() = delete;

    ~Singleton() = delete;

    static T& GetInstance() {
      // int pthread_once(pthread_once_t *once_control, void (*init_routine) (void))；
      // 功能：本函数使用初值为PTHREAD_ONCE_INIT的once_control
      // 变量保证init_routine()函数在本进程执行序列中仅执行一次。
      pthread_once(&ponce_, &Singleton::Init);
      assert(value_ != NULL); 
      return *value_;
    } 

  private:
    static void Init() {
      value_ = new T();
      if (!detail::has_no_destroy<T>::value) {
        //功 能: 注册终止函数(即main执行结束后调用的函数) 
        ::atexit(Destroy);  //如果没有no_destory成员
      }
    }
   
    static void Destroy() {
      typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
      T_must_be_complete_type dummy;
      delete value_;
      value_ = NULL;
    }
};
//对模板类的静态成员初始化
template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

}//namespace qingyi
#endif //QINGYI_UTIL_SINGLETON_H

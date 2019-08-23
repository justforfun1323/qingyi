#ifndef QINGYI_UTIL_CALLBACKS_H
#define QINGYI_UTIL_CALLBACKS_H

#include <functional>
#include <memory>

namespace qingyi {

template<typename CLASS, typename... ARGS>
class WeakCallback {
  private:
    std::weak_ptr<CLASS> object_;
    std::function<void(CLASS*, ARGS)> function_;

  public:
    WeakCallback(const std::weak_ptr<CLASS>& object, 
                  const std::function<void(CLASS*, ARGS)>& function)
    : object_(object), function_(function) {

    } 
 
    void operator () (ARGS&& ... args) const {
      std::shared_ptr<CLASS> ptr(object_.lock());
      if (ptr) {
        function_(ptr.get(), std::forward<ARGS>(args)...);
      }
    }
};

template<typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const std::shared_ptr<CLASS>& object,
                                              void (CLASS::*function)(ARGS...))
{
  return WeakCallback<CLASS, ARGS...>(object, function);
}

template<typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const std::shared_ptr<CLASS>& object,
                                              void (CLASS::*function)(ARGS...) const)
{
  return WeakCallback<CLASS, ARGS...>(object, function);
}


}//namespace qingyi

#endif//QINGYI_UTIL_CALLBACKS_H

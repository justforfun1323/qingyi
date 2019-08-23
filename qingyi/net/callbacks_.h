#ifndef QINGYI_NET_CALLBACKS_H
#define QINGYI_NET_CALLBACKS_H

#include "qingyi/util/timestamp.h"

#include <memory>
#include <functional>
#include <assert.h>

namespace qingyi {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

template<typename T> 
inline T* GetPointer(const std::shared_ptr<T>& ptr) {
  return ptr.get();
}

template<typename T>
inline T* GetPointer(const std::unique_ptr<T>& ptr) {
  return ptr.get();
}

template<typename To, typename From>
inline ::std::shared_ptr<To> DownPointerCase(const ::std::shared_ptr<From>& f) {
  //专门实现shared_ptr之间的转换
  return ::std::static_pointer_cast<To> (f);
}
namespace net {

class Buffer;
class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void()> TimerCallback;

typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;

typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;

typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;

typedef std::function<void(const TcpConnectionPtr&, size_t)>  HighWaterMarkCallback;

typedef std::function<void(const TcpConnectionPtr&,
                           Buffer*,
                           Timestamp)> MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);

void defaultMessageCallback(const TcpConnectionPtr& conn,
			    Buffer* buffer,
                            Timestamp receiveTime);

}//namespace net

}//namespace qingyi

#endif//QINGYI_NET_CALLBACKS_H

#ifndef QINGYI_BASE_TYPES_H
#define QINGYI_BASE_TYPES_H

#include <string.h>
#include <stdint.h>
#include <string>

namespace qingyi {

using std::string;  

inline void MemZero(void *addr, int len) {
  memset(addr, 0, len);
}

template<typename To, typename From>           //I think it's useless
inline To implict_cast(const From& f){
  return f;
}
 
}//namespace qingyi
#endif //QINGYI_BASE_TYPES_H

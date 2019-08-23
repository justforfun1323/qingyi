#ifndef QINGYI_BASE_NONCOPYABLE_H
#define QINGYI_BASE_NONCOPYABLE_H

namespace qingyi {

class noncopyable {
 public:
  noncopyable(const noncopyable&) = delete;
  void operator = (const noncopyable&) = delete;
 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

} // namespace qingyi

#endif //QINGYI_BASE_NONCOPYABLE_H

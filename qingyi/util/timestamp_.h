#ifndef QINGYI_UTIL_TIMESTAMP_H
#define QINGYI_UTIL_TIMESTAMP_H

#include "qingyi/util/copyable_.h"
#include "qingyi/util/type_.h"

namespace qingyi {
class TimeStamp : copyable {
  private:
    int64_t micro_seconds_from_epoch_;

  public:
    TimeStamp() : micro_seconds_from_epoch_(0) { }

    explicit TimeStamp(int64_t micro_seconds_from_epoch_arg)
	: micro_seconds_from_epoch_(micro_seconds_from_epoch_arg) 
        { }

    void Swap(TimeStamp& rhs) {
      std::swap(micro_seconds_from_epoch_, rhs.micro_seconds_from_epoch_);
    }
 
    string ToString() const;

    string ToFormatString(bool show_micro_seconds = true);    
 
    bool Valid() {
      return micro_seconds_from_epoch_ > 0;
    }
 
    int64_t micro_seconds_from_epoch() const {
      return micro_seconds_from_epoch_;
    } 

    time_t SecondsFromEpoch() const {
      return static_cast<time_t> (micro_seconds_from_epoch_ / kMicroSecondsPerSecond);
    }

    static TimeStamp Now();

    static TimeStamp Invalid() {
      return TimeStamp();
    }

    static TimeStamp FromUnixTime(time_t t) {
      return FromUnixTime(t, 0);
    }
   
    static TimeStamp FromUnixTime(time_t t, int micro_seconds) {
      return TimeStamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + micro_seconds);
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000; 
};

inline bool operator < (TimeStamp lhs, TimeStamp rhs) {
  return lhs.micro_seconds_from_epoch() < rhs.micro_seconds_from_epoch();
}

inline bool operator == (TimeStamp lhs, TimeStamp rhs) {
  return lhs.micro_seconds_from_epoch() == rhs.micro_seconds_from_epoch();
}

inline double TimeDifference(TimeStamp high, TimeStamp low) {
  int64_t  diff = high.micro_seconds_from_epoch() - low.micro_seconds_from_epoch();
  return static_cast<double> (diff) / TimeStamp::kMicroSecondsPerSecond;
}

inline TimeStamp AddTime(TimeStamp timestamp, double secs) {
  int64_t delta = static_cast<int64_t> (secs * TimeStamp::kMicroSecondsPerSecond);
  return TimeStamp(timestamp.micro_seconds_from_epoch() + delta);
}

}//namespace qingyi

#endif //QINGYI_TUIL_TIMESTAMP_H

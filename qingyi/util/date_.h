#ifndef QINGYI_UTIL_DATE_H
#define QINGYI_UTIL_DATE_H

#include "qingyi/util/copyable_.h"
#include "qingyi/util/type_.h"

//struct tm {
//int tm_sec; /* 秒 – 取值区间为[0,59] */
//int tm_min; /* 分 - 取值区间为[0,59] */
//int tm_hour; /* 时 - 取值区间为[0,23] */
//int tm_mday; /* 一个月中的日期 - 取值区间为[1,31] */
//int tm_mon; /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
//int tm_year; /* 年份，其值等于实际年份减去1900 */
//int tm_wday; /* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
//int tm_yday; /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
//int tm_isdst; /* 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的时候，tm_isdst为0；不了解情况时，tm_isdst()为负。
//long int tm_gmtoff; /*指定了日期变更线东面时区中UTC东部时区正秒数或UTC西部时区的负秒数*/
//const char *tm_zone; /*当前时区的名字(与环境变量TZ有关)*/
//};
struct tm;

namespace qingyi{

class Date : copyable {
  private:
    int julian_day_number_;
  public:

    struct YearMonthDay {
      int year; //1900 -- 2500 
      int month; // 1 -- 12
      int day; // 1--31
    };

  static const int kDaysPerWeek = 7; 
  static const int kJulianDayOf1970_01_01; 
  Date() : julian_day_number_(0) { }
  
  Date(int year, int month, int day);

  explicit Date(int julian_day_number) 
   : julian_day_number_(julian_day_number) { }
  
  Date(const struct tm& t);

  void Swap(Date& rhs) { 
    std::swap(julian_day_number_, rhs.julian_day_number_);
  }
 
  bool Valid() {
    return julian_day_number_ > 0;
  } 

  string ToIsoString() const;
  
  struct YearMonthDay ToYearMonthDay() const;
 
  int Year() {
    return ToYearMonthDay().year;
  }

  int Month() {
    return ToYearMonthDay().month;
  }
 
  int Day() {
    return ToYearMonthDay().day;
  }

  //[0, 1, 2 ... 6] -> [Sunday, Monday, ... ,Staurday]
  int WeekDay() const {
    return (julian_day_number_ + 1) % kDaysPerWeek;
  }

  int get_julian_day_number() const {
    return julian_day_number_;
  }

};

inline bool operator < (Date lhs, Date rhs) {
  return lhs.get_julian_day_number() < rhs.get_julian_day_number();
}

inline bool operator == (Date lhs, Date rhs) {
  return lhs.get_julian_day_number() == rhs.get_julian_day_number();
}

}//namespace qingyi

#endif//QINGYI_UTIL_DATE_H

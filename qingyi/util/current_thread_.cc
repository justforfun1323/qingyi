#include "qingyi/util/current_thread_.h"
namespace qingyi{
namespace currentthread {
__thread int t_cached_tid = 0;
__thread char t_tid_string[32];
__thread int t_tid_string_length;
__thread const char* t_thread_name = "unknow";

string stackTrace(bool demangel) {
  return string("I don't know this function");
}
} // namespace currentthread
}//namespace qingyi

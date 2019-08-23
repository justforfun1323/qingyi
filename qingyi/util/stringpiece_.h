#ifndef QINGYI_BASE_STRINGPIECE_H
#define QINGYI_BASE_STRINGPIECE_H

#include "qingyi/util/type_.h"

#include <string.h>
#include <iosfwd>


namespace qingyi { 

class StringArg {
  public:
    StringArg(const char* str) 
    : str_(str) 
    { }
    
    StringArg(const string& str)
    : str_(str.c_str())
    { }
     
    const char* c_str() const {
      return str_;
    }
  private:
    const char* str_;
};

class StringPiece {
  private:
    const char* ptr_;
    int length_;

  public:
    StringPiece()
    : ptr_(NULL), length_(0)
    { }
    
    StringPiece(const char* str)
    : ptr_(str), 
      length_(static_cast<int>(strlen(ptr_)))
      { }
      
    StringPiece(const unsigned char* str)
    : ptr_(reinterpret_cast<const char*>(str)),
      length_(static_cast<int>(strlen(ptr_))) 
     { } 
 
    StringPiece(const string& str)
    : ptr_(str.c_str()), 
      length_(static_cast<int>(str.size()))
    { }
  
    StringPiece(const char* offset, unsigned int len)
    : ptr_(offset), length_(len) 
    { }

    const char* data() const { return ptr_; }

    const int size() const { return length_; }
 
    bool empty() { return length_ == 0; }

    const char* begin() { return ptr_; }

    const char* end() { return ptr_ + length_; }

    void clear() { ptr_ = NULL; length_ = 0; }
  
    void set(const char* buffer, unsigned int len) {
      ptr_ = buffer;
      length_ = len;
    }

    void set(const char* buf) {
      ptr_ = buf;
      length_ = strlen(buf);
    }

    char operator[] (unsigned int i) const  {
      return ptr_[i]; 
    }
 
    void remove_prefix(unsigned int n) {
      ptr_ += n;
      length_ -= n; 
    }

    void remove_suffix(unsigned int n) {
      length_ -= n;
    }
  
    bool operator == (const StringPiece& rhs) {
      return (length_ == rhs.length_ && 
             (memcmp(ptr_, rhs.ptr_, length_) == 0)); 
    }
  
    bool operator != (const StringPiece& rhs) {
      return !(*this == rhs);
    }
  
    int Compare(const StringPiece& rhs) {
      int r = memcmp(this->ptr_, rhs.ptr_, this->length_ < rhs.length_ ? this->length_ : rhs.length_); 
      if (r == 0) {
        if (this->length_ < rhs.length_) r = -1;
        else if (this->length_ > rhs.length_) r = +1;
      }
      return r;
    }
    
    string AsString() {
      return string(data(), size());
    }
   
    void CopyToString(string* str) {
      str->assign(ptr_, length_);
    }

    bool StartWith(const StringPiece& rhs) const {
      return ((this->length_ >= rhs.length_) && (memcmp(this->ptr_, rhs.ptr_, rhs.length_) == 0));
    }
};
}//namespace qingyi
#endif //QINGYI_BASE_STRINGPIECE

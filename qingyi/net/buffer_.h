#ifndef QINGYI_NET_BUFFER_H
#define QINGYI_NET_BUFFER_H

#include "qingyi/util/copyable_.h"
#include "qingyi/util/stringpiece_.h"
#include "qingyi/util/type_.h"
#include "qingyi/net/endian_.h"

#include <algorithm>
#include <vector>
//#############
#include <iostream>
//#############
#include <assert.h>
#include <string.h>
namespace qingyi {

namespace net {

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
/////
///// @code
///// +-------------------+------------------+------------------+
///// | prependable bytes |  readable bytes  |  writable bytes  |
///// |                   |     (CONTENT)    |                  |
///// +-------------------+------------------+------------------+
///// |                   |                  |                  |
///// 0      <=      readerIndex   <=   writerIndex    <=     size

class Buffer : copyable {
  private:
   std::vector<char> buffer_;
   size_t reader_index_;
   size_t writer_index_;
   static const char kCRLF[];
  
  public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
    : buffer_(kCheapPrepend + initialSize),
      reader_index_(kCheapPrepend),
      writer_index_(kCheapPrepend) {
      assert(ReadableBytes() == 0);
      assert(WriteableBytes() == initialSize);
      assert(PrependableBytes() == kCheapPrepend);
    }
  
    size_t GetWriter() const {
      return writer_index_; 
    }
   
    void Swap(Buffer& rhs) {
      buffer_.swap(rhs.buffer_);
      std::swap(reader_index_, rhs.reader_index_);
      std::swap(writer_index_, rhs.writer_index_);
    }

    size_t ReadableBytes() const { 
      return writer_index_ - reader_index_; 
    }

    size_t WriteableBytes() const { 
      return buffer_.size() - writer_index_; 
    }

    size_t PrependableBytes() const { 
      return reader_index_; 
    }

    const char* Peek() const { 
      return Begin() + reader_index_; 
    }

    const char* FindCRLF() const {
      const char* crlf = std::search(Peek(), BeginWrite(), kCRLF, kCRLF + 2);
      return crlf == BeginWrite() ? NULL : crlf;
    }
   
    const char* FindCRLF(const char* start) const {
      assert(Peek() <= start);
      assert(start <= BeginWrite());
      const char* crlf = std::search(Peek(), BeginWrite(), kCRLF, kCRLF + 2);
      return crlf == BeginWrite() ? NULL : crlf;
    }

    const char* FindEOL() {
      const void* eol = memchr(Peek(), '\n', ReadableBytes());
      return static_cast<const char*>(eol);
    }
  
    const char* FindEOL(const char* start) {
      assert(start >= Peek());
      assert(start <= BeginWrite());
      const void* eol = memchr(Peek(), '\n', BeginWrite() - start);
      return static_cast<const char*>(eol);
    }

    void Retrieve(size_t len) {
      assert(len <= ReadableBytes()); 
      if (len < ReadableBytes()) {
        reader_index_ += len;
      }
      else {
        RetrieveAll(); 
      }
    }

    void RetrieveAll() {
      reader_index_ = kCheapPrepend;
      writer_index_ = kCheapPrepend;
    }

    void RetrieveUntil(const char* end) {
      assert(end >= Peek());
      assert(end <= BeginWrite());
      Retrieve(end - Peek());
    }

    void RetrieveInt64() {
      Retrieve(sizeof(int64_t));
    }    

    void RetrieveInt32() {
      Retrieve(sizeof(int32_t));
    }

    void RetrieveInt16() {
      Retrieve(sizeof(int16_t));
    }

    void RetrieveInt8() {
      Retrieve(sizeof(int8_t));
    }
  
    string RetrieveAllAsString() {
      return RetrieveAsString(ReadableBytes()); 
    } 
   
    string RetrieveAsString(size_t len) {
      assert(len <= ReadableBytes());
      string result(Peek(), len);
      Retrieve(len);
      return result;
    }

    StringPiece ToStringPiece() const {
      return StringPiece(Peek(), ReadableBytes());
    }

    void Append(const StringPiece& str) { 
      Append(str.data(), str.size());
    }

    void Append(const char* data, size_t len) {
      EnsureWriteableBytes(len);
      std::copy(data, data + len, BeginWrite());
      HasWritten(len);
    }
  
    void Append(const void* data, size_t len) {
      Append(static_cast<const char*> (data), len);
    }

    void EnsureWriteableBytes(size_t len) {
      if (WriteableBytes() < len) {
        MakeSpace(len);
      }
      assert(WriteableBytes() >= len);
    }

    char* BeginWrite()  {
      return Begin() + writer_index_;
    }
  
    const char* BeginWrite() const {
      return Begin() + writer_index_;
    }

    void HasWritten(size_t len) {
      assert(len <= WriteableBytes());
      writer_index_ += len;
    }

    void UnWrite(size_t len) {
      assert(len <= ReadableBytes());
      writer_index_ -= len;
    }
  
    void AppendInt64(int64_t x) {
      int64_t be64 = sockets::HostToNetwork64(x);
      Append(&be64, sizeof(be64));
    }

    void AppendInt32(int32_t x) {
      int32_t be32 = sockets::HostToNetwork32(x);
      Append(&be32, sizeof(be32));
    }
  
    void AppendInt16(int16_t x) {
      int16_t be16 = sockets::HostToNetwork16(x);
      Append(&be16, sizeof(be16));
    }

    void AppendInt8(int8_t x) {
      Append(&x, sizeof(x));
    }

    int64_t ReadInt64() {
      int64_t res = PeekInt64();
      RetrieveInt64();
      return res;
    }

    int32_t ReadInt32() {
      int32_t res = PeekInt32(); 
      RetrieveInt32();
      return res;
    }

    int16_t ReadInt16() {
      int16_t res = PeekInt16();
      RetrieveInt16();
      return res;
    }
   
    int8_t ReadInt8() { 
      int8_t res = PeekInt8();
      RetrieveInt8();
      return res;
    }

    int64_t PeekInt64() const {
      assert(ReadableBytes() >= sizeof(int64_t));
      int64_t be64 = 0;
      ::memcpy(&be64, Peek(), sizeof(be64));
      return sockets::NetworkToHost64(be64);
    }

    int32_t PeekInt32() const {
      assert(ReadableBytes() >= sizeof(int32_t));
      int32_t be32 = 0;
      ::memcpy(&be32, Peek(), sizeof(be32));
      return sockets::NetworkToHost32(be32);
    }

    int16_t PeekInt16() const {
      assert(ReadableBytes() >= sizeof(int16_t));
      int16_t be16 = 0;
      ::memcpy(&be16, Peek(), sizeof(be16));
      return sockets::NetworkToHost16(be16);
    }

    int8_t PeekInt8() const {
      assert(ReadableBytes() >= sizeof(int8_t));
      int8_t x = *Peek();
      return x;
    }

    void PrependInt64(int64_t x) {
      int64_t be64 = sockets::HostToNetwork64(x);
      Prepend(&be64, sizeof(be64));
    }

    void PrependInt32(int32_t x) {
      int32_t be32 = sockets::HostToNetwork32(x);
      Prepend(&be32, sizeof(be32));
    }

    void PrependInt16(int16_t x) {
      int16_t be16 = sockets::HostToNetwork16(x);
      Prepend(&be16, sizeof(be16));
    }

    void PrepenInt8(int8_t x) {
      Prepend(&x, sizeof(x));
    }

    void Prepend(const void* data, size_t len) {
      assert(len <= PrependableBytes());
      reader_index_ -= len;
      const char* d =static_cast<const char*>(data);
      std::copy(d, d + len, Begin() + reader_index_);
    }

    void Shrink(size_t reserve) {
      Buffer other;
      other.EnsureWriteableBytes(ReadableBytes() + reserve);
      other.Append(ToStringPiece());
      Swap(other);
    }

    size_t InteralCapacity() const {
      return buffer_.capacity();
    }

    ssize_t ReadFd(int fd, int* saved_errorno);

    private:
      const char *Begin() const {
        return &*buffer_.begin(); 
      }
 
      char* Begin() {
        return &*buffer_.begin();
      }

      void MakeSpace(size_t len) {
        if (WriteableBytes() + PrependableBytes() < len + kCheapPrepend) {
          buffer_.resize(writer_index_ + len);
        }
        else {
          assert(kCheapPrepend < reader_index_);
          size_t readable = ReadableBytes();
          std::copy(Begin() + reader_index_,
                    Begin() + writer_index_,
                    Begin() + kCheapPrepend);
          reader_index_ = kCheapPrepend;
          writer_index_ = reader_index_ + readable;
          assert(readable == ReadableBytes());
        }
      }
};
}//namespace net

}//namespace qingyi
#endif//QINGYI_NET_BUFFER_H

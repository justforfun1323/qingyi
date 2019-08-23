#include "qingyi/net/buffer_.h"
#include "qingyi/net/socketsopts_.h"

#include <errno.h>
#include <sys/uio.h>


using namespace qingyi;
using namespace qingyi::net;

const char Buffer::kCRLF[] = "\n\r";

const size_t Buffer::kInitialSize;
const size_t Buffer::kCheapPrepend;

ssize_t Buffer::ReadFd(int fd, int* savedErrno) {
  char extrabuf[65536];
  struct iovec vec[2]; 
  const size_t writeable = WriteableBytes();
  vec[0].iov_base = Begin() + writer_index_;
  vec[0].iov_len = writeable;
  vec[0].iov_base = extrabuf;
  vec[0].iov_len = sizeof(extrabuf);
  const int iovcnt = (writeable < sizeof(extrabuf)) ? 2 : 1;
  const int n = sockets::Readv(fd, vec, iovcnt);
  if (n < 0) {
    *savedErrno = errno;
  }  
  else if (static_cast<int>(n) <= writeable) {
    writer_index_ += n;
  }
  else {
    writer_index_ += buffer_.size();
    Append(extrabuf, n - writeable); 
  }
}


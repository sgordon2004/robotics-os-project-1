#include "rix/ipc/pipe.hpp"

namespace rix {
namespace ipc {

// Factory method to create a pair of Pipe objects.
// The first element is the read-end and the second is the write-end.
std::array<Pipe, 2> Pipe::create() {
    int fds[2]; // array to store the pipes
    if (::pipe(fds) == -1) {
        // Handle error - return invalid pipes
        return {Pipe(), Pipe()};
    }
    // fds[0] is read end; fds[1] is write-end
    return {Pipe(fds[0], true), Pipe(fds[1], false)};
}

Pipe::Pipe() : File(), read_end_(false) {}

// Copy constructor - duplicates the underlying file descriptopr using `dup`
Pipe::Pipe(const Pipe &src) : File(), read_end_(src.read_end_) {
    if (src.fd_ >= 0 ) { // if the file is valid
            fd_ = ::dup(src.fd_);
        }
}

// Assignment operator - duplicates the underlying file descriptors using `dup`
Pipe &Pipe::operator=(const Pipe &src) {
    if (this != &src) { // avoids self-assignment
        if (fd_ > 0) { // if the file is valid
            ::close(fd_); // close existing fd
            fd_ = -1;
        }
        if (src.fd_ >= 0) { // if the source file descriptor is valid
            fd_ = ::dup(src.fd_);
        } else {
            fd_ = -1;
        }
        read_end_ = src.read_end_; // copy read_end
    }
    return *this;
}

Pipe::Pipe(Pipe &&other) : File(std::move(other)), read_end_(other.read_end_) {}

Pipe &Pipe::operator=(Pipe &&other) {
    if (fd_ > 0) {
        ::close(fd_);
        fd_ = -1;
        read_end_ = false;
    }
    std::swap(fd_, other.fd_);
    read_end_ = std::move(other.read_end_);
    return *this;
}

Pipe::~Pipe() {}

bool Pipe::is_read_end() const { return read_end_; }

bool Pipe::is_write_end() const { return !read_end_; }

Pipe::Pipe(int fd, bool read_end) : File(fd), read_end_(read_end) {}

}  // namespace ipc
}  // namespace rix
#include "rix/ipc/file.hpp"

namespace rix {
namespace ipc {

/**< TODO */
bool File::remove(const std::string &pathname) {
    return false;
}

File::File() : fd_(-1) {}

File::File(int fd) : fd_(fd) {}

/**< TODO */
File::File(std::string pathname, int creation_flags, mode_t mode) {}

/**< TODO */
File::File(const File &src) {}

/**< TODO */
File &File::operator=(const File &src) { return *this; }

File::File(File &&src) : fd_(-1) { std::swap(fd_, src.fd_); }

File &File::operator=(File &&src) {
    if (fd_ > 0) {
        ::close(fd_);
        fd_ = -1;
    }
    std::swap(fd_, src.fd_);
    return *this;
}

/**< TODO */
File::~File() {}

/**< TODO */
ssize_t File::read(uint8_t *buffer, size_t size) const {
    return -1;
}

/**< TODO */
ssize_t File::write(const uint8_t *buffer, size_t size) const {
    return -1;
}

int File::fd() const { return fd_; }

/**< TODO */
bool File::ok() const {
    return false;
}

/**< TODO */
void File::set_nonblocking(bool status) {}

/**< TODO */
bool File::is_nonblocking() const {
    return false;
}

/**< TODO */
bool File::wait_for_writable(const util::Duration &duration) const {
    return false;
}

/**< TODO */
bool File::wait_for_readable(const util::Duration &duration) const {
    return false;
}

}  // namespace ipc
}  // namespace rix
#include "rix/ipc/file.hpp"
#include <fcntl.h>
#include <unistd.h>

/*
The File class should implement the IO interface using a file descriptor and system calls.
You must implement the member functions of the File class in this file.
The expected behavior of each function is defined in the header file `include/rix/ipc/file.hpp`.
*/

namespace rix {
namespace ipc {

/**< TODO */
// Removes the file specified by `pathname`
// Returns true if removed successfully
bool File::remove(const std::string &pathname) {
    return ::unlink(pathname.c_str()) == 0;
}

// Constructor definitions for File class
File::File() : fd_(-1) {} // Default constructor 

File::File(int fd) : fd_(fd) {} // Constructor taking a file descriptor

// Creates a File object by opening the file  specified by the path
File::File(std::string pathname, int creation_flags, mode_t mode) {
    // Use ::open() to call the POSIX system call
    // Convert the std::string to a C string with .c_str()
    // Store the returned file descriptor (int) in `fd_`
    // open() returns -1 on error, or a valid descriptor (>= 0) on success
    fd_ = ::open(pathname.c_str(), creation_flags, mode);
}

// Copy constructor - duplicates the underlying file descriptor using `dup`
// dup associates a second file descriptor witha currently open file.
// Called when a new object is being created from an existing object.
File::File(const File &src) : fd_(-1) {
    if (src.fd_ >= 0 ) { // if the file is valid
        fd_ = ::dup(src.fd_);
    }
}

// Assignment operator - duplicates the underlying file using `dup`
// Called when an existing object is being reassigned.
File &File::operator=(const File &src) {
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
    }
    return *this;
}

File::File(File &&src) : fd_(-1) { std::swap(fd_, src.fd_); }

File &File::operator=(File &&src) {
    if (fd_ > 0) {
        ::close(fd_);
        fd_ = -1;
    }
    std::swap(fd_, src.fd_);
    return *this;
}

// Destructor - closes the underlying file descriptor
File::~File() {
    if (fd_ >= 0) { // only close if valid
        ::close(fd_);
    } 
}

// Reads `size` bytes from the file and stores them in `dst`
ssize_t File::read(uint8_t *dst, size_t size) const {
    if (fd_ >= 0) { // only read if valid
        return ::read(fd_, dst, size);
    }
    return -1; // returns -1 on error
}

// Write `size` bytes from `src` to the file
ssize_t File::write(const uint8_t *src, size_t size) const {
    if (fd_ >= 0) { // only write if valid
        return ::write(fd_, src, size);
    }
    return -1;
}

int File::fd() const { return fd_; }

// Returns true if file is in valid state; false otherwise
bool File::ok() const {
    if (fd_ >= 0) {
        return true;
    }
    return false;
}

// Toggles non-blocking IO operations
// status flag - true for non-blocking, false for blocking
void File::set_nonblocking(bool status) {
    if (fd_ >= 0) { // only if file is valid
        int flags = ::fcntl(fd_, F_GETFL, 0); // Get the current flags
        if (status) { // non-blocking
            flags |= O_NONBLOCK; // Enable non-blocking
        } else { // blocking
            flags &= ~O_NONBLOCK; // Disable non-blocking
        }
        ::fcntl(fd_, F_SETFL, flags); // Set new flags
    }
}

// Returns true if the file is in non-blocking mode
bool File::is_nonblocking() const {
    if (fd_ >= 0) { // only if the file is valid
        int flags = ::fcntl(fd_, F_GETFL, 0);
        return (flags & O_NONBLOCK) != 0; // Check if flag is set
    }
    return false;
}

// Waits the specified duration for the file to become writable
// Return true if the file has become writable within the duration
bool File::wait_for_writable(const util::Duration &duration) const {
    if (fd_ >= 0) { // only if file is valid
        struct pollfd pfd;
        pfd.fd = fd_;
        pfd.events = POLLOUT; // wait for writeability
        pfd.revents = 0;

        // Convert duration to milliseconds
        int timeout_ms = duration.to_milliseconds();

        int result = ::poll(&pfd, 1, timeout_ms);

        // poll() returns > 0 if event occurred, 0 if timeout, -1 on error
        if (result > 0 && pfd.revents & POLLOUT) {
            return true;
        }
    }
    return false;
}


// Waits the specified duration for the file to become readable
// Return true if the file has become readable within the duration
bool File::wait_for_readable(const util::Duration &duration) const {
    if (fd_ >= 0) { // only if file is valid
        struct pollfd pfd;
        pfd.fd = fd_;
        pfd.events = POLLIN; // wait for readability
        pfd.revents = 0;

        // Convert duration to milliseconds
        int timeout_ms = duration.to_milliseconds();

        int result = ::poll(&pfd, 1, timeout_ms);

        // poll() returns > 0 if event occurred, 0 if timeout, -1 on error
        if (result > 0 && pfd.revents & POLLIN) {
            return true;
        }
    }
    return false;
}

}  // namespace ipc
}  // namespace rix
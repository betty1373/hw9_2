#ifndef Async_H
#define Async_H
#include <cstddef>

namespace async {

    using handle_t = void *;
    handle_t connect(std::size_t bulk);
    void receive(handle_t handle, const char *data, std::size_t size);
    void disconnect(handle_t handle);
}
#endif
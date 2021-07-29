#include "../inc/async.h"
#include "../inc/Context.h"
#include "../inc/Logger.h"

#include <map>
#include <memory>
namespace async {
    std::map<handle_t,std::shared_ptr<Context>> contexts;
    std::mutex context_mutex;

static handle_t AddContext(const std::shared_ptr<Context>& context) {
    std::lock_guard<std::mutex> lock(context_mutex);
    contexts.emplace(std::make_pair(context.get(),context));
    return context.get();
}
static void NewData(handle_t handle, const char *data, std::size_t size) {
    std::shared_ptr<Context> context;
    {
        std::lock_guard<std::mutex> lock(context_mutex);
        auto it = contexts.find(handle);
        if (it!=contexts.end()) {
            context = it->second;
        }
    }
    if (context) {
        context->NewData(data,size);
    }
}
static void DeleteContext(handle_t handle) {
    std::lock_guard<std::mutex> lock(context_mutex);
    auto it = contexts.find(handle);
    if (it!=contexts.end()) {
        contexts.erase(it);
    }
}
handle_t connect(std::size_t bulk) {
    auto m_context =  std::make_shared<Context>(bulk);
    return AddContext(m_context);
}

void receive(handle_t handle, const char *data, std::size_t size) {
    NewData(handle,data,size);
}

void disconnect(handle_t handle) {
    DeleteContext(handle);
}

}

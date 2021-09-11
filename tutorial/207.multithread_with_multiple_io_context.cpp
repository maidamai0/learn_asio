#include <atomic>
#include <cstddef>
#include <memory>
#include <thread>
#include <vector>
#include "asio/executor_work_guard.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"

using io_context_t = asio::io_context;
using work_guard_t = asio::executor_work_guard<asio::io_context::executor_type>;

class io_context_group {
 public:
  io_context_group(const size_t size) {
    for (size_t i = 0; i < size; ++i) {
      io_contexts_.emplace_back(std::make_shared<io_context_t>());
      work_guards_.emplace_back(std::make_shared<work_guard_t>());
    }
  }

  void run() {
    for (auto& ctx : io_contexts_) {
      threads_.emplace_back(std::thread([&ctx]() { ctx->run(); }));
    }

    for (auto& t : threads_) {
      t.join();
    }
  }

  io_context_t& query() { return *io_contexts_[index++ % io_contexts_.size()]; }

 private:
  template <typename T>
  using vector_ptr = std::vector<std::shared_ptr<T>>;

  vector_ptr<io_context_t> io_contexts_;
  vector_ptr<work_guard_t> work_guards_;
  std::vector<std::thread> threads_;

  std::atomic<std::size_t> index = 0;
};

int main() {
  io_context_group group(std::thread::hardware_concurrency());
  asio::ip::tcp::socket socket(group.query());
  group.run();
  return 0;
}
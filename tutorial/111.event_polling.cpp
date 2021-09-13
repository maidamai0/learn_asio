#include "asio/executor_work_guard.hpp"
#include "asio/io_context.hpp"

class application {
 public:
  application(asio::io_context& io_context) { (void)io_context; }
  void poll_message() {}
  bool closed() const { return false; }
};

int main() {
  asio::io_context io;
  auto work_guard = asio::make_work_guard(io);
  application app(io);
  while (!app.closed()) {
    io.poll();
    app.poll_message();
  }
  return 0;
}
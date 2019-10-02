#include <asio/io_context.hpp>
#include <asio/system_timer.hpp>

#include <fmt/core.h>
#include <chrono>
#include <functional>
#include <iostream>

class printer {
 public:
  printer(asio::io_context& io_ctx)
      : io_ctx_{io_ctx}, timer_{io_ctx_, std::chrono::seconds(1)} {
    timer_.async_wait(std::bind(&printer::print, this));
  }

  ~printer() { fmt::print("Fire!\n"); }

  void print() {
    if (count_ > 0) {
      fmt::print("{}\n", count_--);
      timer_.expires_at(timer_.expiry() + std::chrono::seconds(1));
      timer_.async_wait(std::bind(&printer::print, this));
    }
  }

  void run() { io_ctx_.run(); }

 private:
  asio::io_context& io_ctx_;
  asio::system_timer timer_;
  unsigned count_{10};
};

int main() {
  asio::io_context io_ctx;
  printer(io_ctx).run();
}
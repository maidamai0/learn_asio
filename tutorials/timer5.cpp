#include <fmt/core.h>
#include <asio/bind_executor.hpp>
#include <asio/io_context.hpp>
#include <asio/strand.hpp>
#include <asio/system_timer.hpp>

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

class Printer {
 public:
  Printer(asio::io_context& io_ctx, bool need_sync = true)
      : io_ctx_{io_ctx},
        strand_{io_ctx_},
        timer1_{io_ctx_, std::chrono::seconds(1)},
        timer2_{io_ctx_, std::chrono::seconds(1)},
        need_sync_{need_sync} {
    if (need_sync_) {
      timer1_.async_wait(
          asio::bind_executor(strand_, std::bind(&Printer::print1, this)));

      timer2_.async_wait(
          asio::bind_executor(strand_, std::bind(&Printer::print2, this)));
    } else {
      timer1_.async_wait(std::bind(&Printer::print1, this));

      timer2_.async_wait(std::bind(&Printer::print2, this));
    }
  }

  ~Printer() { fmt::print("Final count is {}\n", cnt_); }

  void print1() {
    if (cnt_ < 10) {
      // fmt::print("{}:{}\n", __PRETTY_FUNCTION__, cnt_++);
      std::cout << __PRETTY_FUNCTION__ << ":" << cnt_++ << "\n";
      timer1_.expires_at(timer1_.expiry() + std::chrono::seconds(1));
      if (need_sync_) {
        timer1_.async_wait(
            asio::bind_executor(strand_, std::bind(&Printer::print1, this)));
      } else {
        timer1_.async_wait(std::bind(&Printer::print1, this));
      }
    }
  };
  void print2() {
    if (cnt_ < 10) {
      // fmt::print("{}:{}\n", __PRETTY_FUNCTION__, cnt_++);
      std::cout << __PRETTY_FUNCTION__ << ":" << cnt_++ << "\n";
      timer2_.expires_at(timer2_.expiry() + std::chrono::seconds(1));

      if (need_sync_) {
        timer2_.async_wait(
            asio::bind_executor(strand_, std::bind(&Printer::print2, this)));
      } else {
        timer2_.async_wait(std::bind(&Printer::print2, this));
      }
    }
  };

 private:
  asio::io_context& io_ctx_;
  asio::io_context::strand strand_;
  asio::system_timer timer1_;
  asio::system_timer timer2_;
  unsigned cnt_{0};
  unsigned need_sync_{true};
};

int main() {
  fmt::print("Synchronize version:\n");
  asio::io_context io_ctx;
  {
    Printer printer(io_ctx);
    io_ctx.run();
    for (size_t i = 0; i < 10; i++) {
      std::thread(&asio::io_context::run, &io_ctx).join();
    }
  }

  fmt::print("No synchronize version:\n");
  asio::io_context io_ctx1;
  {
    Printer printer1(io_ctx1, false);
    io_ctx1.run();
    for (size_t i = 0; i < 10; i++) {
      std::thread(&asio::io_context::run, &io_ctx1).join();
    }
  }
}
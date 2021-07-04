#include <asio/bind_executor.hpp>
#include <asio/io_context.hpp>
#include <asio/strand.hpp>
#include <asio/system_timer.hpp>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

#include "cxxopts.hpp"

using namespace std::chrono_literals;
constexpr auto kCount = 100;
constexpr auto kStep = 1us;

class Printer {
 public:
  Printer(asio::io_context& io_ctx, bool need_sync = true)
      : io_ctx_{io_ctx}, strand_{io_ctx_}, timer1_{io_ctx_, kStep}, timer2_{io_ctx_, kStep}, need_sync_{need_sync} {
    if (need_sync_) {
      timer1_.async_wait(asio::bind_executor(strand_, std::bind(&Printer::print1, this)));
      timer2_.async_wait(asio::bind_executor(strand_, std::bind(&Printer::print2, this)));
    } else {
      timer1_.async_wait(std::bind(&Printer::print1, this));
      timer2_.async_wait(std::bind(&Printer::print2, this));
    }
  }

  ~Printer() { std::cout << "Final count is " << cnt_ << '\n'; }

  void print1() {
    if (cnt_ < kCount) {
      std::cout << std::this_thread::get_id() << " " << __PRETTY_FUNCTION__ << ":" << cnt_++ << "\n";
      timer1_.expires_at(timer1_.expiry() + kStep);
      if (need_sync_) {
        timer1_.async_wait(asio::bind_executor(strand_, std::bind(&Printer::print1, this)));
      } else {
        timer1_.async_wait(std::bind(&Printer::print1, this));
      }
    }
  };

  void print2() {
    if (cnt_ < kCount) {
      std::cout << std::this_thread::get_id() << " " << __PRETTY_FUNCTION__ << ":" << cnt_++ << "\n";
      timer2_.expires_at(timer2_.expiry() + kStep);

      if (need_sync_) {
        timer2_.async_wait(asio::bind_executor(strand_, std::bind(&Printer::print2, this)));
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

int main(int argc, char** argv) {
  auto sync = false;
  cxxopts::Options options(APP_NAME, " - asio strand test");
  // clang-format off
  options
  .add_options()
  ("s,sync", "Sync with strand", cxxopts::value<bool>(sync));
  cxxopts::ParseResult args;
  // clang-format on
  try {
    args = options.parse(argc, argv);
  } catch (const cxxopts::OptionException& e) {
    std::cout << e.what() << '\n';
    std::cout << options.help() << '\n';
    return EXIT_FAILURE;
  }

  if (!sync) {
    std::cout << "No ";
  }
  std::cout << "Synchronize version:\n";
  asio::io_context io_ctx;
  {
    Printer printer(io_ctx, sync);
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 10; i++) {
      threads.emplace_back(std::thread(&asio::io_context::run, &io_ctx));
    }
    io_ctx.run();

    for (auto& thread : threads) {
      thread.join();
    }
  }
  std::cout << "main thread is: " << std::this_thread::get_id() << '\n';
}
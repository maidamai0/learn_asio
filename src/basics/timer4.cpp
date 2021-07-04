#include <asio/io_context.hpp>
#include <asio/system_timer.hpp>
#include <chrono>
#include <functional>
#include <iostream>

#include "asio/error_code.hpp"
#include "common/log.hpp"

class printer {
 public:
  printer(asio::io_context& io_ctx) : timer_{io_ctx, std::chrono::seconds(1)} {
    timer_.async_wait(std::bind(&printer::print, this, std::placeholders::_1));
  }

  ~printer() { LOGI("Fire!"); }

  void print(asio::error_code) {
    if (count_ > 0) {
      LOGI("{}", count_--);
      timer_.expires_at(timer_.expiry() + std::chrono::seconds(1));
      timer_.async_wait(std::bind(&printer::print, this, std::placeholders::_1));
    }
  }

 private:
  asio::system_timer timer_;
  unsigned count_{5};
};

class Logger {
 public:
  void log(asio::error_code ec) {
    if (ec) {
      LOGE("error:{}", ec.message());
    } else {
      LOGI("{}", __PRETTY_FUNCTION__);
    }
  }
};

int main() {
  asio::io_context io_ctx;
  asio::system_timer timer(io_ctx, std::chrono::seconds(8));
  Logger log;
  timer.async_wait(std::bind(&Logger::log, &log, std::placeholders::_1));
  printer p(io_ctx);
  io_ctx.run();
}
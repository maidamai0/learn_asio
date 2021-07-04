#include <asio/io_context.hpp>
#include <asio/system_timer.hpp>
#include <chrono>
#include <functional>
#include <string_view>

#include "common/log.hpp"

void handler(asio::error_code, std::string_view name) {
  TRACE;
  LOGI("handle {}", name);
}

int main() {
  using namespace std::chrono_literals;
  using namespace std::placeholders;
  asio::io_context io;
  LOGI("io context created");

  asio::system_timer timer(io);
  timer.expires_after(1s);
  timer.async_wait(std::bind(handler, _1, "async timer"));
  LOGI("async timer ready");

  LOGI("sync timer ready");
  asio::system_timer timer1(io, 2s);
  timer1.async_wait(std::bind(handler, _1, "sync timer"));

  LOGI("io context begin to run");
  io.run();
}
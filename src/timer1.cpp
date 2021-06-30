#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <chrono>
#include <thread>

#include "log.hpp"

int main() {
  using namespace std::chrono_literals;
  asio::io_context io;
  asio::steady_timer timer(io, 2s);
  LOGI("timer created");
  std::this_thread::sleep_for(1s);

  LOGI("timer.wait");
  timer.wait();
  LOGI("timer expired");

  return 0;
}
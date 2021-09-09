#include <iostream>
#include <thread>

#include "asio/executor_work_guard.hpp"
#include "asio/io_context.hpp"
#include "asio/post.hpp"

using work_gurad_t = asio::executor_work_guard<asio::io_context::executor_type>;
using namespace std::chrono_literals;

int main() {
  asio::io_context io_context;
  auto work_guaard = asio::make_work_guard(io_context);
  asio::post(io_context, []() { std::cout << "post from main thread" << std::endl; });
  asio::post(io_context, []() {
    std::this_thread::sleep_for(2s);
    std::cout << "post from another thread" << std::endl;
  });
  io_context.run();

  return 0;
}
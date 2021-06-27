#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <chrono>
#include <iostream>

int main() {
  asio::io_context io_ctx;
  asio::steady_timer timer(io_ctx, std::chrono::seconds(5));

  std::cout << "timer begins wait...\n";
  timer.wait();
  std::cout << "Hello world" << std::endl;

  return 0;
}
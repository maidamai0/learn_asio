#include <array>
#include <cstddef>
#include <cstdint>
#include <thread>
#include <type_traits>
#include <vector>

#include "asio/io_context.hpp"

int main() {
  asio::io_context io_context;
  const auto count = std::thread::hardware_concurrency() * 2;
  std::vector<std::thread> threads;

  for (std::remove_const_t<decltype(count)> i = 0; i < count; ++i) {
    threads.emplace_back([&io_context]() { io_context.run(); });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  return 0;
}
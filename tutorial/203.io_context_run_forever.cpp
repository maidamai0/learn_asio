#include <chrono>
#include <iostream>
#include <thread>
#include "asio/executor_work_guard.hpp"
#include "asio/io_context.hpp"

using work_guard_t = asio::executor_work_guard<asio::io_context::executor_type>;

int main() {
  asio::io_context io_context;
  work_guard_t work_gurad(io_context.get_executor());

  std::thread watchdog([&]() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    io_context.stop();
  });

  io_context.run();
  std::cout << "Job's done! Restart\n";
  io_context.restart();
  io_context.run_one();
  std::cout << "Job's done! Continue to execution\n";
}
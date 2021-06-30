#include <asio/io_context.hpp>
#include <asio/system_timer.hpp>
#include <chrono>
#include <iostream>
#include <thread>

#include "log.hpp"

asio::io_context io_context;
asio::system_timer timer1(io_context);
asio::system_timer timer2(io_context);

void timer1_hander(asio::error_code ec) {
  (void)ec;

  static size_t cnt = 0;
  std::cout << "Thread is is " << std::this_thread::get_id() << " " << __FUNCTION__ << " cnt is " << cnt++ << std::endl;

  if (cnt < 10) {
    timer1.expires_after(std::chrono::seconds(10));
    timer1.async_wait(timer1_hander);
    std::thread([]() { io_context.run(); }).detach();
  }
}

void timer2_handler(asio::error_code ec) {
  (void)ec;
  static size_t cnt = 0;
  std::cout << "Thread is is " << std::this_thread::get_id() << " " << __FUNCTION__ << " cnt is " << cnt++ << std::endl;

  if (cnt < 10) {
    timer2.expires_after(std::chrono::seconds(100));
    timer2.async_wait(timer2_handler);

    std::thread([]() { io_context.run(); }).detach();
  }
}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  std::cout << "This thread id is " << std::this_thread::get_id() << " " << __FUNCTION__ << std::endl;

  timer1.expires_after(std::chrono::seconds(2));
  timer1.async_wait(timer1_hander);

  timer2.expires_after(std::chrono::seconds(2));
  timer2.async_wait(timer2_handler);

  std::thread([]() { io_context.run(); }).join();
}

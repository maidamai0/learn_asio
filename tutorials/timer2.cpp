#include <asio/io_context.hpp>
#include <asio/system_timer.hpp>

#include <fmt/core.h>
#include <fmt/format.h>

#include <chrono>

void handler(asio::error_code) { fmt::print("Hello world\n"); }

int main() {
  asio::io_context io_ctx;
  asio::system_timer timer(io_ctx);

  timer.expires_after(std::chrono::seconds(2));
  timer.async_wait(&handler);

  /// @warning   asio::system_timer (io_ctx,
  /// std::chrono::seconds(5)).async_wait(&handler); will not work
  asio::system_timer timer1(io_ctx, std::chrono::seconds(5));
  timer1.async_wait(&handler);

  io_ctx.run();
}
#include <asio/io_context.hpp>
#include <asio/system_timer.hpp>

#include <fmt/core.h>

#include <functional>
#include <iostream>

/**
 * @brief mimic std::bind
 *
 */
class wrapper {
 public:
  wrapper(asio::system_timer& timer, unsigned& count)
      : timer_{timer}, count_{count} {
    fmt::print("{}\n", __PRETTY_FUNCTION__);
  }

  ~wrapper() { fmt::print("{}\n", __PRETTY_FUNCTION__); }
  wrapper(wrapper&& rhs) : timer_{rhs.timer_}, count_{rhs.count_} {
    fmt::print("{}\n", __PRETTY_FUNCTION__);
  }
  wrapper(const wrapper& rhs) : timer_{rhs.timer_}, count_{rhs.count_} {
    fmt::print("{}\n", __PRETTY_FUNCTION__);
  }

  wrapper& operator=(const wrapper&) {
    fmt::print("{}\n", __PRETTY_FUNCTION__);
    return *this;
  }

  template <class ARG> void operator()(ARG) {
    if (count_ < 5) {
      fmt::print("{}\n", count_++);
      timer_.expires_at(timer_.expiry() + std::chrono::seconds(1));
      timer_.async_wait(wrapper(timer_, count_));
    }
  }

 private:
  asio::system_timer& timer_;
  unsigned& count_;
};

void handler(asio::error_code ec, asio::system_timer& timer, unsigned& count) {
  (void)ec;

  if (count < 5) {
    fmt::print("{}\n", count++);
    timer.expires_at(timer.expiry() + std::chrono::seconds(1));
    timer.async_wait(std::bind(handler, std::placeholders::_1, std::ref(timer),
                               std::ref(count)));
  }
}

int main() {
  asio::io_context io_ctx;
  asio::system_timer timer(io_ctx, std::chrono::seconds(1));

  unsigned cnt = 0;
  timer.async_wait(std::bind(handler, std::placeholders::_1, std::ref(timer),
                             std::ref(cnt)));
  //   timer.async_wait(wrapper(timer, cnt));
  io_ctx.run();

  fmt::print("Final count is {}\n", cnt);

  return 0;
}
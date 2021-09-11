#include <cstddef>
#include <thread>

#include "asio/bind_executor.hpp"
#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/read.hpp"
#include "asio/strand.hpp"
#include "asio/streambuf.hpp"
#include "asio/write.hpp"

using socket_t = asio::ip::tcp::socket;
using strand_t = asio::io_context::strand;
using buffer_t = asio::streambuf;

class session {
 public:
  session(asio::io_context& io_context)
      : socket_(io_context), read_strand_(io_context), write_strand_(io_context) {}

  void async_read() {
    asio::async_read(socket_, read_buffer_,
                     asio::bind_executor(read_strand_, [&](const asio::error_code& error,
                                                           std::size_t bytes_transferred) {
                       (void)bytes_transferred;
                       (void)error;
                     }));
  }

  void async_write() {
    asio::async_write(socket_, write_buffer_,
                      asio::bind_executor(write_strand_, [&](const asio::error_code& error,
                                                             std::size_t bytes_tansferred) {
                        (void)bytes_tansferred;
                        (void)error;
                      }));
  }

 private:
  socket_t socket_;
  strand_t read_strand_;
  strand_t write_strand_;
  buffer_t read_buffer_;
  buffer_t write_buffer_;
};

int main() {
  return 0;
}
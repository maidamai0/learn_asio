#pragma once

#include <array>
#include <cstddef>
#include <memory>

#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "common/macro.hpp"
#include "request.h"
#include "request_handler.h"
#include "response.h"

namespace http {
namespace server {
class connection_manager;

class connection : public std::enable_shared_from_this<connection> {
 public:
  using pointer = std::shared_ptr<connection>;
  static constexpr auto buffer_size = 1024;
  no_copy(connection);
  connection(asio::io_context& io_context, connection_manager& manager, request_handler& handler);
  asio::ip::tcp::socket& socket();
  void start();
  void stop();

 private:
  void dispatch();
  void handle_read(const asio::error_code&, std::size_t bytes_transferred);
  void handle_write(const asio::error_code& ec);

 private:
  asio::ip::tcp::socket socket_;
  connection_manager& connection_manager_;
  request_handler& request_handler_;
  std::array<char, buffer_size> buffer_;
  request request_;
  response response_;
};
}  // namespace server
}  // namespace http
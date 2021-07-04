#include "connection.h"

#include <functional>

#include "asio/error.hpp"
#include "asio/error_code.hpp"
#include "asio/ip/tcp.hpp"
#include "connection_manager.h"

namespace http {
namespace server {
connection::connection(asio::io_context& io_context, connection_manager& manager,
                       request_handler& handler)
    : socket_(io_context), connection_manager_(manager), request_handler_(handler) {}

asio::ip::tcp::socket& connection::socket() {
  return socket_;
}

void connection::start() {
  socket_.async_read_some(asio::buffer(buffer_),
                          std::bind(&connection::handle_read, shared_from_this(),
                                    std::placeholders::_1, std::placeholders::_2));
}

void connection::stop() {
  socket_.close();
}

void connection::handle_read(const asio::error_code& ec, std::size_t bytes_transferred) {
  if (ec && ec != asio::error::operation_aborted) {
    connection_manager_.stop(shared_from_this());
    return;
  }

  // TODO (tonghao): 2021-07-04
  // complete this
  (void)bytes_transferred;
  response_ = response::stock_response(response::status_code::not_implemented);
  socket_.async_write_some(response_.to_buffers(),
                           std::bind(&connection::handle_write, this, std::placeholders::_1));
}

void connection::handle_write(const asio::error_code& ec) {
  if (ec && ec != asio::error::operation_aborted) {
    connection_manager_.stop(shared_from_this());
    return;
  }

  socket_.shutdown(asio::ip::tcp::socket::shutdown_both);
}

}  // namespace server
}  // namespace http
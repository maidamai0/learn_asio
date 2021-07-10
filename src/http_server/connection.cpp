#include "connection.h"

#include <exception>
#include <functional>
#include <system_error>

#include "asio/error.hpp"
#include "asio/error_code.hpp"
#include "asio/ip/tcp.hpp"
#include "common/log.hpp"
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

  const auto ret = request_parser_.parse(buffer_.data(), bytes_transferred);
  if (ret == message_status::want_more) {
    return start();
  } else if (ret == message_status::faild) {
    // TODO (tonghao): 2021-07-10
    // complete me
    return;
  }

  // TODO (tonghao): 2021-07-10
  // complete this
  response_ = response::stock_response(response::status_code::not_implemented);
  socket_.async_write_some(response_.to_buffers(),
                           std::bind(&connection::handle_write, this, std::placeholders::_1));
}

void connection::handle_write(const asio::error_code& ec) {
  if (ec && ec != asio::error::operation_aborted) {
    connection_manager_.stop(shared_from_this());
    return;
  }

  try {
    socket_.shutdown(asio::ip::tcp::socket::shutdown_both);
  } catch (const std::exception& e) {
    LOGW("shutdown socket failed:{}", e.what());
  }
}

}  // namespace server
}  // namespace http
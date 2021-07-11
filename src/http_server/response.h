#pragma once

#include <vector>

#include "asio/buffer.hpp"
#include "common/macro.hpp"
#include "common/message.h"

namespace http::server {
struct response : public http_message {
  enum class status_code {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
  };

  response(const status_code code);
  response();
  no_copy(response);

  status_code status = status_code::not_implemented;
  std::string content;
  std::vector<asio::const_buffer> to_buffers();
};
}  // namespace http::server
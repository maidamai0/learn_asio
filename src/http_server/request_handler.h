#pragma once

#include <string>
#include <string_view>

#include "common/macro.hpp"

namespace http {
namespace server {

class request;
class response;

class request_handler {
 public:
  no_copy(request_handler);
  explicit request_handler(const std::string_view doc_root) : doc_root_(doc_root){};

  virtual void on_get(const request& req, response& rsp);
  virtual void on_post(const request& req, response& rsp);
  virtual void on_put(const request& req, response& rsp);
  virtual void on_delete(const request& req, response& rsp);

 private:
  std::string_view doc_root_;
};
}  // namespace server
}  // namespace http
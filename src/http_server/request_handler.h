#pragma once

#include <string>
#include <string_view>

#include "common/macro.hpp"

namespace http {
namespace server {

struct request;
struct reply;

class request_handler {
 public:
  no_copy(request_handler);
  // TODO (tonghao): 2021-07-04
  // complete this
  explicit request_handler(const std::string_view doc_root) : doc_root_(doc_root){};
  reply handle_request(const request& req);

 private:
  static std::string decode_url(const std::string& url);

 private:
  std::string_view doc_root_;
};
}  // namespace server
}  // namespace http
#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>

#include "common/message.h"

namespace http {
namespace server {
class request_parser : public http_message {
 public:
  request_parser() = default;
  void parse(const char* data, size_t len);
  std::string print() const;

 private:
  std::string url_;
};
}  // namespace server
}  // namespace http
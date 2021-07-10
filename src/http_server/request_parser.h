#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "common/message.h"

namespace http {
namespace server {
class request_parser : public http_message {
 public:
  request_parser() = default;
  message_status parse(const char* data, std::size_t len);
  std::string print() const;
};
}  // namespace server
}  // namespace http
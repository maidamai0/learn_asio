#pragma once

namespace http {
namespace server {
class request;
class request_parser {
 public:
  // TODO (tonghao): 2021-07-04
  // complete this
  request_parser() = default;
  void reset();
  void parse();

 private:
  void consume();
};
}  // namespace server
}  // namespace http
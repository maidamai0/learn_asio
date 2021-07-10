#include "message.h"

#include <cstddef>
#include <memory>
#include <string>

#include "common/log.hpp"
#include "http_parser.h"
#include "magic_enum.hpp"
#include "spdlog/spdlog.h"

[[maybe_unused]] static const char request_data[] =
    "POST /joyent/http-parser HTTP/1.1\r\n"
    "Host: github.com\r\n"
    "DNT: 1\r\n"
    "Accept-Encoding: gzip, deflate, sdch\r\n"
    "Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4\r\n"
    "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) "
    "AppleWebKit/537.36 (KHTML, like Gecko) "
    "Chrome/39.0.2171.65 Safari/537.36\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
    "image/webp,*/*;q=0.8\r\n"
    "Referer: https://github.com/joyent/http-parser\r\n"
    "Connection: keep-alive\r\n"
    "Transfer-Encoding: chunked\r\n"
    "Cache-Control: max-age=0\r\n\r\n12\r\nhello world";

[[maybe_unused]] static const char response_data[] = R"(HTTP/1.1 200 OK
Date: Sun, 10 Oct 2010 23:26:07 GMT
Server: Apache/2.2.8 (Ubuntu) mod_ssl/2.2.8 OpenSSL/0.9.8g
Last-Modified: Sun, 26 Sep 2010 22:04:35 GMT
ETag: "45b6-834-49130cc1182c0"
Accept-Ranges: bytes
Content-Length: 12
Connection: close
Content-Type: text/html

// Hello world!)";

namespace http::server {
class http_message::impl {
 public:
  impl(http_message& api) : api_(api) {}
  http_parser parser_;
  http_parser_settings settings_;
  http_message& api_;

  void reset() {
    http_parser_init(&parser_, HTTP_BOTH);
    parser_.data = &api_;

    http_parser_settings_init(&settings_);
    settings_.on_body = on_body;
    settings_.on_chunk_complete = on_chunk_complete;
    settings_.on_chunk_header = on_chunk_header;
    settings_.on_header_field = on_header_field;
    settings_.on_header_value = on_header_value;
    settings_.on_headers_complete = on_headers_complete;
    settings_.on_message_complete = on_message_complete;
    settings_.on_message_begin = on_message_begin;
    settings_.on_status = on_status;
    settings_.on_url = on_url;
  }

  void parse(const char* data, size_t len) {
    http_parser_execute(&parser_, &settings_, data, len);
    const auto error = static_cast<http_errno>(parser_.http_errno);
    if (error != http_errno::HPE_OK) {
      api_.message_status_ = message_status::faild;
      LOGW("error is {}", http_errno_description(error));
    }
  }

  static int on_message_begin(http_parser* parser) {
    (void)parser;
    return 0;
  }

  static int on_url(http_parser* parser, const char* at, size_t len) {
    auto* message = static_cast<http_message*>(parser->data);
    message->url_ += std::string(at, len);
    return 0;
  }

  static int on_status(http_parser* parser, const char* at, size_t len) {
    (void)parser;
    (void)at;
    (void)len;
    return 0;
  }

  static int on_header_field(http_parser* parser, const char* at, size_t len) {
    auto* message = static_cast<http_message*>(parser->data);
    message->save_headers();
    message->current_header_field_ += std::string(at, len);
    return 0;
  }

  static int on_header_value(http_parser* parser, const char* at, size_t len) {
    auto* message = static_cast<http_message*>(parser->data);
    message->current_header_value_ += std::string(at, len);
    return 0;
  }

  static int on_headers_complete(http_parser* parser) {
    (void)parser;

    auto* message = static_cast<http_message*>(parser->data);
    message->save_headers();
    return 0;
  }

  static int on_body(http_parser* parser, const char* at, size_t len) {
    auto* message = static_cast<http_message*>(parser->data);
    message->body_ += std::string(at, len);
    return 0;
  }

  static int on_message_complete(http_parser* parser) {
    (void)parser;
    TRACE;
    auto* message = static_cast<http_message*>(parser->data);
    message->message_status_ = message_status::complete;
    return 0;
  }

  static int on_chunk_header(http_parser* parser) {
    (void)parser;
    TRACE;
    return 0;
  }

  static int on_chunk_complete(http_parser* parser) {
    TRACE;
    (void)parser;
    return 0;
  }
};

http_message::http_message() : pimpl_(std::make_unique<http_message::impl>(*this)) {
  reset();
}

http_message::~http_message() = default;

message_status http_message::parse(const char* data, size_t len) {
  (void)data;
  (void)len;
  // for (size_t i = 0; i < sizeof(request_data); ++i) {
  pimpl_->parse(data, len);
  // }

  if (message_status_ == message_status::complete) {
    http_major_ = pimpl_->parser_.http_major;
    http_minor_ = pimpl_->parser_.http_minor;

    const auto _method = static_cast<http_method>(pimpl_->parser_.method);
    switch (_method) {
      case http_method::HTTP_GET:
        method_ = method::GET;
        break;
      case http_method::HTTP_POST:
        method_ = method::POST;
        break;
      case http_method::HTTP_PUT:
        method_ = method::PUT;
        break;
      case http_method::HTTP_DELETE:
        method_ = method::DELETE;
        break;
      default:
        LOGW("unknow http method:{}", http_method_str(_method));
    }
    status_ = static_cast<http_status>(pimpl_->parser_.status_code);
    keep_alive_ = http_should_keep_alive(&pimpl_->parser_);
    upgrade_ = pimpl_->parser_.upgrade;
    print();
  }

  return message_status_;
}

void http_message::print() {
  LOGI("http version: {}.{}", http_major_, http_minor_);
  LOGI("http method: {}", magic_enum::enum_name(method_));
  LOGI("http status: {}", status_);
  LOGI("http body is {}", body_);
  LOGI("http body size is {}", body_.size());
  LOGI("is websocket? {}", upgrade_ ? "yes" : "no");
  LOGI("http should keep alive? {}", keep_alive_);
}

void http_message::reset() {
  pimpl_->reset();
}

void http_message::save_headers() {
  if (!current_header_value_.empty() && !current_header_field_.empty()) {
    headers_[current_header_field_] = current_header_value_;
    current_header_field_.clear();
    current_header_value_.clear();
  }
}

}  // namespace http::server
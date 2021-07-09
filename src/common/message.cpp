#include "message.h"

#include <string>

#include "common/log.hpp"
#include "http_parser.h"

// static const char request_data[] =
//     "POST /joyent/http-parser HTTP/1.1\r\n"
//     "Host: github.com\r\n"
//     "DNT: 1\r\n"
//     "Accept-Encoding: gzip, deflate, sdch\r\n"
//     "Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4\r\n"
//     "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) "
//     "AppleWebKit/537.36 (KHTML, like Gecko) "
//     "Chrome/39.0.2171.65 Safari/537.36\r\n"
//     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,"
//     "image/webp,*/*;q=0.8\r\n"
//     "Referer: https://github.com/joyent/http-parser\r\n"
//     "Connection: keep-alive\r\n"
//     "Transfer-Encoding: chunked\r\n"
//     "Cache-Control: max-age=0\r\n\r\n\r\nhello world\r\n\r\n";

// static const char response_data[] = R"(HTTP/1.1 200 OK
// Date: Sun, 10 Oct 2010 23:26:07 GMT
// Server: Apache/2.2.8 (Ubuntu) mod_ssl/2.2.8 OpenSSL/0.9.8g
// Last-Modified: Sun, 26 Sep 2010 22:04:35 GMT
// ETag: "45b6-834-49130cc1182c0"
// Accept-Ranges: bytes
// Content-Length: 12
// Connection: close
// Content-Type: text/html

// Hello world!)";

namespace http::server {
class http_message::impl {
 public:
  static int on_message_begin([[maybe_unused]] http_parser* parser) {
    TRACE;
    return 0;
  }

  static int on_url(http_parser* parser, const char* at, size_t len) {
    TRACE;
    auto* message = static_cast<http_message*>(parser->data);
    message->url_ = std::string(at, len);
    return 0;
  }

  static int on_status(http_parser* parser, const char* at, size_t len) {
    (void)parser;
    (void)at;
    (void)len;
    TRACE;
    LOGI("{}", std::string(at, len));
    return 0;
  }

  static int on_header_field(http_parser* parser, const char* at, size_t len) {
    TRACE;
    auto* message = static_cast<http_message*>(parser->data);
    message->current_header_ = std::string(at, len);
    return 0;
  }

  static int on_header_value(http_parser* parser, const char* at, size_t len) {
    TRACE;
    auto* message = static_cast<http_message*>(parser->data);
    message->headers_[message->current_header_] = std::string(at, len);
    return 0;
  }

  static int on_headers_complete(http_parser* parser) {
    (void)parser;
    TRACE;
    return 0;
  }

  static int on_body(http_parser* parser, const char* at, size_t len) {
    TRACE;
    auto* message = static_cast<http_message*>(parser->data);
    message->body_start_ = at;
    message->bodu_len_ = len;
    return 0;
  }

  static int on_message_complete(http_parser* parser) {
    (void)parser;
    TRACE;
    return 0;
  }

  static int on_chunk_header(http_parser* parser) {
    (void)parser;
    TRACE;
    return 0;
  }

  static int on_chunk_complete(http_parser* parser) {
    (void)parser;
    TRACE;
    return 0;
  }
};

void http_message::parse(const char* data, size_t len) {
  unsigned long version = http_parser_version();
  unsigned major = (version >> 16) & 255;
  unsigned minor = (version >> 8) & 255;
  unsigned patch = version & 255;
  LOGI("http_parser {}.{}.{}", major, minor, patch);

  http_parser parser;
  http_parser_init(&parser, HTTP_BOTH);
  parser.data = this;

  http_parser_settings settings;
  http_parser_settings_init(&settings);
  settings.on_body = impl::on_body;
  settings.on_chunk_complete = impl::on_chunk_complete;
  settings.on_chunk_header = impl::on_chunk_header;
  settings.on_header_field = impl::on_header_field;
  settings.on_header_value = impl::on_header_value;
  settings.on_headers_complete = impl::on_headers_complete;
  settings.on_message_complete = impl::on_message_complete;
  settings.on_message_begin = impl::on_message_begin;
  settings.on_status = impl::on_status;
  settings.on_url = impl::on_url;

  const auto succeed = http_parser_execute(&parser, &settings, data, len);

  http_major = parser.http_major;
  http_minor = parser.http_minor;

  LOGI("http version: {}.{}", parser.http_major, parser.http_minor);
  LOGI("http method: {}", http_method_str(static_cast<http_method>(parser.method)));
  LOGI("http status: {}", http_status_str(static_cast<http_status>(parser.status_code)));
  LOGI("is websocket? {}", parser.upgrade ? "yes" : "no");
  LOGI("http should keep alive? {}", http_should_keep_alive(&parser));
  LOGI("http is chunk? {}", http_body_is_final(&parser));

  const auto error = static_cast<http_errno>(parser.http_errno);
  LOGI("http error:{} [{}] ", http_errno_name(error), http_errno_description(error));

  LOGI("{}", succeed);
}

}  // namespace http::server
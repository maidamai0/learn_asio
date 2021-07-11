#include "request_handler.h"

#include "common/log.hpp"
#include "request.h"
#include "response.h"

namespace http::server {
void request_handler::on_get(const request& req, response& rsp) {
  (void)req;
  (void)rsp;
  rsp.content = "complete me";
  rsp.status = response::status_code::ok;
  TRACE;
}

void request_handler::on_post(const request& req, response& rsp) {
  (void)req;
  (void)rsp;
  rsp.content = "complete me";
  rsp.status = response::status_code::ok;
  TRACE;
}

void request_handler::on_put(const request& req, response& rsp) {
  (void)req;
  (void)rsp;
  rsp.content = "complete me";
  rsp.status = response::status_code::ok;
  TRACE;
}

void request_handler::on_delete(const request& req, response& rsp) {
  (void)req;
  (void)rsp;
  rsp.content = "complete me";
  rsp.status = response::status_code::ok;
  TRACE;
}
}  // namespace http::server
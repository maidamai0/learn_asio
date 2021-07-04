#include "common/log.hpp"
#include "cxxopts.hpp"
#include "server.h"

int main(int argc, char** argv) {
  try {
    int port = 5000;
    cxxopts::Options options("http_server", "A HTTP server");
    options.add_options()("p,port", "Server TCP port", cxxopts::value<int>(port));
    options.parse(argc, argv);
    http::server::server server(port, "/");
    LOGI("server running on {}", port);
    server.run();
  } catch (const cxxopts::OptionException& e) {
    LOGE("{}", e.what());
  }
  return 0;
}
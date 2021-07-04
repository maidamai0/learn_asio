#include <asio/io_context.hpp>
#include <asio/signal_set.hpp>
#include <iostream>

void signal_handler(const asio::error_code& err, int signal_num) {
  static unsigned int_num = 0;
  static unsigned term_num = 0;
  if (!err) {
    switch (signal_num) {
      case SIGINT:
        std::cout << "got signal SIGINT " << int_num << " times" << std::endl;
        break;
      case SIGTERM: {
        std::cout << "got signal SIGTERM " << term_num << " times" << std::endl;
        break;
      }
      default:
        break;
    }  // switch
  }    // check err
}

int main() {
  asio::io_context iocxt;
  asio::signal_set signals(iocxt, SIGINT, SIGTERM);
  signals.async_wait(signal_handler);
  iocxt.run();
}
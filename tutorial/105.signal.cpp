#include "asio/io_context.hpp"
#include "asio/signal_set.hpp"

#include <cstdlib>
#include <iostream>

void handler(const asio::error_code& ec, int signal_number) {
  std::cout << "Caught signal: " << signal_number << "\n";
  if (ec) {
    std::cout << "Error: " << ec.message() << "\n";
  }

  switch (signal_number) {
    case SIGILL:
      std::cout << "Illegal instruction\n";
      break;
    case SIGINT:
      std::cout << "Interrupted\n";
      break;
    case SIGTERM:
      std::cout << "Terminated\n";
      break;
  }
}

int main() {
  asio::io_context io;
  asio::signal_set signals(io, SIGILL, SIGINT, SIGTERM);
  signals.async_wait(handler);

  io.run();

  return EXIT_SUCCESS;
}
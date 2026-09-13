#include <network/server/server.hpp>

#include <boost/asio.hpp>

#include <iostream>
#include <string>

using boost::asio::awaitable;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

namespace {

awaitable<void> handleSession(tcp::socket socket) {
  boost::asio::streambuf buffer;
  try {
    for (;;) {
      co_await boost::asio::async_read_until(socket, buffer, "\n", use_awaitable);

      std::istream is(&buffer);
      std::string line;
      std::getline(is, line);
      line.push_back('\n');

      co_await boost::asio::async_write(socket, boost::asio::buffer(line), use_awaitable);
    }
  } catch (const boost::system::system_error& e) {
    if (e.code() != boost::asio::error::eof) {
      std::cerr << "[join-server] session error: " << e.what() << '\n';
    }
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  std::uint16_t port = 9000;

  try {
    if (argc > 1) {
      port = static_cast<std::uint16_t>(std::stoi(argv[1]));
    }
  } catch (const std::exception& e) {
    std::cerr << "Invalid argument: " << e.what() << '\n';
    return 1;
  }

  boost::asio::io_context io;
  TcpServer server(io, {.port = port, .on_session = [](tcp::socket socket) { return handleSession(std::move(socket)); }});
  server.run();
  io.run();

  return 0;
}

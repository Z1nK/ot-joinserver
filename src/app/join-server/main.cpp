#include <network/command-handler/command_handler.hpp>
#include <network/protocol/parser.hpp>
#include <network/protocol/response.hpp>
#include <network/server/server.hpp>

#include <boost/asio.hpp>

#include <iostream>
#include <string>

using boost::asio::awaitable;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

namespace {

awaitable<void> handleSession(tcp::socket socket, CommandHandler& handler) {
  boost::asio::streambuf buffer;
  try {
    for (;;) {
      co_await boost::asio::async_read_until(socket, buffer, "\n", use_awaitable);

      std::istream is(&buffer);
      std::string line;
      std::getline(is, line);

      ParseResult result = parseCommand(line);
      std::string response = result.command ? handler.handle(*result.command) : formatError(result.error);

      co_await boost::asio::async_write(socket, boost::asio::buffer(response), use_awaitable);
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

  DbEngine engine;
  CommandHandler handler(engine);

  engine.createTable("A");
  engine.createTable("B");

  boost::asio::io_context io;
  TcpServer server(io, {.port = port,
                         .on_session = [&handler](tcp::socket socket) { return handleSession(std::move(socket), handler); }});
  server.run();
  io.run();

  return 0;
}

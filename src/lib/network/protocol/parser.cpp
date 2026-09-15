#include "parser.hpp"

#include <cctype>
#include <charconv>
#include <vector>

namespace {

std::vector<std::string_view> tokenize(std::string_view line) {
  std::vector<std::string_view> tokens;
  size_t pos = 0;
  while (pos < line.size()) {
    while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos]))) {
      ++pos;
    }
    size_t start = pos;
    while (pos < line.size() && !std::isspace(static_cast<unsigned char>(line[pos]))) {
      ++pos;
    }
    if (pos > start) {
      tokens.emplace_back(line.substr(start, pos - start));
    }
  }
  return tokens;
}

ParseResult error(std::string message) { return {std::nullopt, std::move(message)}; }

ParseResult ok(Command command) { return {std::move(command), {}}; }

std::optional<int> parseInt(std::string_view token) {
  int value = 0;
  auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), value);
  if (ec != std::errc{} || ptr != token.data() + token.size()) {
    return std::nullopt;
  }
  return value;
}

}  // namespace

ParseResult parseCommand(std::string_view line) {
  std::vector<std::string_view> tokens = tokenize(line);
  if (tokens.empty()) {
    return error("empty command");
  }

  std::string_view keyword = tokens[0];

  if (keyword == "INSERT") {
    if (tokens.size() != 4) {
      return error("INSERT requires exactly 3 arguments: table id name");
    }
    std::optional<int> id = parseInt(tokens[2]);
    if (!id) {
      return error("INSERT id must be an integer");
    }
    return ok(InsertCommand{std::string(tokens[1]), *id, std::string(tokens[3])});
  }
  if (keyword == "TRUNCATE") {
    if (tokens.size() != 2) {
      return error("TRUNCATE requires exactly 1 argument: table");
    }
    return ok(TruncateCommand{std::string(tokens[1])});
  }
  if (keyword == "CREATE_TABLE") {
    if (tokens.size() != 2) {
      return error("CREATE_TABLE requires exactly 1 argument: table");
    }
    return ok(CreateTableCommand{std::string(tokens[1])});
  }
  if (keyword == "PRINT_TABLE") {
    if (tokens.size() != 2) {
      return error("PRINT_TABLE requires exactly 1 argument: table");
    }
    return ok(PrintTableCommand{std::string(tokens[1])});
  }
  if (keyword == "INTERSECTION") {
    if (tokens.size() != 1) {
      return error("INTERSECTION takes no arguments");
    }
    return ok(IntersectionCommand{});
  }
  if (keyword == "SYMMETRIC_DIFFERENCE") {
    if (tokens.size() != 1) {
      return error("SYMMETRIC_DIFFERENCE takes no arguments");
    }
    return ok(SymmetricDifferenceCommand{});
  }
  if (keyword == "LEFT_JOIN") {
    if (tokens.size() != 1) {
      return error("LEFT_JOIN takes no arguments");
    }
    return ok(LeftJoinCommand{});
  }
  if (keyword == "RIGHT_JOIN") {
    if (tokens.size() != 1) {
      return error("RIGHT_JOIN takes no arguments");
    }
    return ok(RightJoinCommand{});
  }
  if (keyword == "FULL_JOIN") {
    if (tokens.size() != 1) {
      return error("FULL_JOIN takes no arguments");
    }
    return ok(FullJoinCommand{});
  }

  return error("unknown command: " + std::string(keyword));
}

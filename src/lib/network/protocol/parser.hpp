#pragma once

#include <network/protocol/command.hpp>

#include <optional>
#include <string>
#include <string_view>

struct ParseResult {
  std::optional<Command> command;
  std::string error;  // set iff command is empty
};

ParseResult parseCommand(std::string_view line);

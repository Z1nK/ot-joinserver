#pragma once

#include <string>
#include <variant>

struct InsertCommand {
  std::string table;
  int id;
  std::string name;
};

struct TruncateCommand {
  std::string table;
};

struct CreateTableCommand {
  std::string table;
};

struct PrintTableCommand {
  std::string table;
};

struct IntersectionCommand {};
struct SymmetricDifferenceCommand {};
struct LeftJoinCommand {};
struct RightJoinCommand {};
struct FullJoinCommand {};

using Command = std::variant<InsertCommand, TruncateCommand, CreateTableCommand, PrintTableCommand,
                              IntersectionCommand, SymmetricDifferenceCommand, LeftJoinCommand, RightJoinCommand,
                              FullJoinCommand>;

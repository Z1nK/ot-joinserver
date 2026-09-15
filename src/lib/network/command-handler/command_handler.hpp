#pragma once

#include <data/db-engine/db_engine.hpp>
#include <network/protocol/command.hpp>

#include <string>

class CommandHandler {
public:
  explicit CommandHandler(DbEngine& engine);

  std::string handle(const Command& command);

  std::string operator()(const InsertCommand& command);
  std::string operator()(const TruncateCommand& command);
  std::string operator()(const CreateTableCommand& command);
  std::string operator()(const PrintTableCommand& command);
  std::string operator()(const IntersectionCommand& command);
  std::string operator()(const SymmetricDifferenceCommand& command);
  std::string operator()(const LeftJoinCommand& command);
  std::string operator()(const RightJoinCommand& command);
  std::string operator()(const FullJoinCommand& command);

private:
  DbEngine& engine_;
};

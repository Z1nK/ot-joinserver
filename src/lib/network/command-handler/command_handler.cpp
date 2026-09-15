#include "command_handler.hpp"

#include <data/join/join.hpp>
#include <network/protocol/response.hpp>

#include <algorithm>
#include <string>

namespace {

using JoinFn = std::vector<JoinedRecord> (*)(const Table&, const Table&);

std::string runJoin(DbEngine& engine, JoinFn joinFn) {
  const Table* tableA = engine.getTable("A");
  const Table* tableB = engine.getTable("B");
  if (!tableA || !tableB) {
    return formatError("tables A and B must both exist");
  }
  return formatRows(joinFn(*tableA, *tableB));
}

std::vector<Record> sortedRecords(const Table& table) {
  std::vector<Record> records = table.getAll();
  std::sort(records.begin(), records.end(), [](const Record& a, const Record& b) { return a.id < b.id; });
  return records;
}

}  // namespace

CommandHandler::CommandHandler(DbEngine& engine) : engine_(engine) {}

std::string CommandHandler::handle(const Command& command) { return std::visit(*this, command); }

std::string CommandHandler::operator()(const InsertCommand& command) {
  Table* table = engine_.getTable(command.table);
  if (!table) {
    return formatError("no such table: " + command.table);
  }
  if (!table->insert(command.id, command.name)) {
    return formatError("duplicate id: " + std::to_string(command.id));
  }
  return formatOk();
}

std::string CommandHandler::operator()(const TruncateCommand& command) {
  Table* table = engine_.getTable(command.table);
  if (!table) {
    return formatError("no such table: " + command.table);
  }
  table->truncate();
  return formatOk();
}

std::string CommandHandler::operator()(const CreateTableCommand& command) {
  if (!engine_.createTable(command.table)) {
    return formatError("table already exists: " + command.table);
  }
  return formatOk();
}

std::string CommandHandler::operator()(const PrintTableCommand& command) {
  const Table* table = engine_.getTable(command.table);
  if (!table) {
    return formatError("no such table: " + command.table);
  }
  return formatRows(sortedRecords(*table));
}

std::string CommandHandler::operator()(const IntersectionCommand&) { return runJoin(engine_, innerJoin); }

std::string CommandHandler::operator()(const SymmetricDifferenceCommand&) {
  return runJoin(engine_, symmetricDifference);
}

std::string CommandHandler::operator()(const LeftJoinCommand&) { return runJoin(engine_, leftJoin); }

std::string CommandHandler::operator()(const RightJoinCommand&) { return runJoin(engine_, rightJoin); }

std::string CommandHandler::operator()(const FullJoinCommand&) { return runJoin(engine_, fullJoin); }

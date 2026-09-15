#include "response.hpp"

std::string formatOk() { return "OK\n\n"; }

std::string formatError(std::string_view message) { return "ERR " + std::string(message) + "\n\n"; }

std::string formatRows(const std::vector<JoinedRecord>& rows) {
  std::string response;
  for (const auto& row : rows) {
    response += std::to_string(row.id);
    response += ',';
    response += row.name_a.value_or("NULL");
    response += ',';
    response += row.name_b.value_or("NULL");
    response += '\n';
  }
  response += "OK\n\n";
  return response;
}

std::string formatRows(const std::vector<Record>& rows) {
  std::string response;
  for (const auto& row : rows) {
    response += std::to_string(row.id);
    response += ',';
    response += row.name;
    response += '\n';
  }
  response += "OK\n\n";
  return response;
}

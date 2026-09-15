#include <data/db-engine/db_engine.hpp>
#include <data/join/join.hpp>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

namespace {

std::vector<Record> sortedRecords(const Table& table) {
  std::vector<Record> records = table.getAll();
  std::sort(records.begin(), records.end(), [](const Record& a, const Record& b) { return a.id < b.id; });
  return records;
}

void printTable(const Table& table, std::string_view title) {
  std::cout << title << '\n';
  std::cout << "id | name\n";
  std::cout << "---+----------\n";
  for (const auto& record : sortedRecords(table)) {
    std::cout << std::setw(2) << record.id << " | " << record.name << '\n';
  }
  std::cout << '\n';
}

void printJoinRows(std::string_view title, const std::vector<JoinedRecord>& rows) {
  std::cout << title << '\n';
  std::cout << "id | name_a     | name_b\n";
  std::cout << "---+------------+----------\n";
  for (const auto& row : rows) {
    std::cout << std::right << std::setw(2) << row.id << " | " << std::left << std::setw(10)
              << row.name_a.value_or("NULL") << " | " << row.name_b.value_or("NULL") << '\n';
  }
  std::cout << '\n';
}

}  // namespace

int main() {
  DbEngine engine;

  engine.createTable("A");
  engine.createTable("B");

  Table* tableA = engine.getTable("A");
  Table* tableB = engine.getTable("B");

  tableA->insert(0, "lean");
  tableA->insert(1, "sweater");
  tableA->insert(2, "frank");
  tableA->insert(3, "violation");
  tableA->insert(4, "quality");
  tableA->insert(5, "precision");

  tableB->insert(3, "proposal");
  tableB->insert(4, "example");
  tableB->insert(5, "lake");
  tableB->insert(6, "flour");
  tableB->insert(7, "wonder");
  tableB->insert(8, "selection");

  std::cout << "Tables: ";
  for (const auto& name : engine.listTables()) {
    std::cout << name << ' ';
  }
  std::cout << "\n\n";

  printTable(*tableA, "Table A");
  printTable(*tableB, "Table B");

  printJoinRows("INNER JOIN A x B ON A.id = B.id", innerJoin(*tableA, *tableB));
  printJoinRows("LEFT JOIN A x B ON A.id = B.id", leftJoin(*tableA, *tableB));
  printJoinRows("RIGHT JOIN A x B ON A.id = B.id", rightJoin(*tableA, *tableB));
  printJoinRows("FULL JOIN A x B ON A.id = B.id", fullJoin(*tableA, *tableB));
  printJoinRows("SYMMETRIC DIFFERENCE JOIN A x B ON A.id = B.id", symmetricDifference(*tableA, *tableB));
  return 0;
}

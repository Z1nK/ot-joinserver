#include <data/db-engine/db_engine.hpp>

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

void printInnerJoin(const Table& left, const Table& right) {
  std::cout << "INNER JOIN A x B ON A.id = B.id\n";
  std::cout << "id | name_a     | name_b\n";
  std::cout << "---+------------+----------\n";
  for (const auto& recordA : sortedRecords(left)) {
    if (auto recordB = right.get(recordA.id)) {
      std::cout << std::right << std::setw(2) << recordA.id << " | " << std::left << std::setw(10) << recordA.name
                << " | " << recordB->name << '\n';
    }
  }
  std::cout << '\n';
}

void printLeftJoin(const Table& left, const Table& right) {
  std::cout << "LEFT JOIN A x B ON A.id = B.id\n";
  std::cout << "id | name_a     | name_b\n";
  std::cout << "---+------------+----------\n";
  for (const auto& recordA : sortedRecords(left)) {
    if (auto recordB = right.get(recordA.id)) {
      std::cout << std::right << std::setw(2) << recordA.id << " | " << std::left << std::setw(10) << recordA.name
                << " | " << recordB->name << '\n';
    } else {
      std::cout << std::right << std::setw(2) << recordA.id << " | " << std::left << std::setw(10) << recordA.name
                << " | " << "NULL" << '\n';
    }
  }
  std::cout << '\n';
}

void printRightJoin(const Table& left, const Table& right) {
  std::cout << "RIGHT JOIN A x B ON A.id = B.id\n";
  std::cout << "id | name_a     | name_b\n";
  std::cout << "---+------------+----------\n";
  for (const auto& recordB : sortedRecords(right)) {
    if (auto recordA = left.get(recordB.id)) {
      std::cout << std::right << std::setw(2) << recordB.id << " | " << std::left << std::setw(10) << recordA->name
                << " | " << recordB.name << '\n';
    } else {
      std::cout << std::right << std::setw(2) << recordB.id << " | " << std::left << std::setw(10) << "NULL"
                << " | " << recordB.name << '\n';
    }
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

  printInnerJoin(*tableA, *tableB);
  printLeftJoin(*tableA, *tableB);
  printRightJoin(*tableA, *tableB);
  return 0;
}

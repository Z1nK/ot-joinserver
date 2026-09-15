#include "join.hpp"

#include <algorithm>

namespace {

std::vector<Record> sortedRecords(const Table& table) {
  std::vector<Record> records = table.getAll();
  std::sort(records.begin(), records.end(), [](const Record& a, const Record& b) { return a.id < b.id; });
  return records;
}

}  // namespace

std::vector<JoinedRecord> innerJoin(const Table& left, const Table& right) {
  std::vector<JoinedRecord> result;
  for (const auto& recordA : sortedRecords(left)) {
    if (auto recordB = right.get(recordA.id)) {
      result.push_back({recordA.id, recordA.name, recordB->name});
    }
  }
  return result;
}

std::vector<JoinedRecord> leftJoin(const Table& left, const Table& right) {
  std::vector<JoinedRecord> result;
  for (const auto& recordA : sortedRecords(left)) {
    auto recordB = right.get(recordA.id);
    result.push_back({recordA.id, recordA.name, recordB ? std::optional(recordB->name) : std::nullopt});
  }
  return result;
}

std::vector<JoinedRecord> rightJoin(const Table& left, const Table& right) {
  std::vector<JoinedRecord> result;
  for (const auto& recordB : sortedRecords(right)) {
    auto recordA = left.get(recordB.id);
    result.push_back({recordB.id, recordA ? std::optional(recordA->name) : std::nullopt, recordB.name});
  }
  return result;
}

std::vector<JoinedRecord> fullJoin(const Table& left, const Table& right) {
  std::vector<JoinedRecord> result;
  for (const auto& recordA : sortedRecords(left)) {
    auto recordB = right.get(recordA.id);
    result.push_back({recordA.id, recordA.name, recordB ? std::optional(recordB->name) : std::nullopt});
  }
  for (const auto& recordB : sortedRecords(right)) {
    if (!left.get(recordB.id)) {
      result.push_back({recordB.id, std::nullopt, recordB.name});
    }
  }
  return result;
}

std::vector<JoinedRecord> symmetricDifference(const Table& left, const Table& right) {
  std::vector<JoinedRecord> result;
  for (const auto& recordA : sortedRecords(left)) {
    if (!right.get(recordA.id)) {
      result.push_back({recordA.id, recordA.name, std::nullopt});
    }
  }
  for (const auto& recordB : sortedRecords(right)) {
    if (!left.get(recordB.id)) {
      result.push_back({recordB.id, std::nullopt, recordB.name});
    }
  }
  return result;
}


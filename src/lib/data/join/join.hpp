#pragma once

#include <data/storage/table.hpp>

#include <optional>
#include <string>
#include <vector>

struct JoinedRecord {
  int id;
  std::optional<std::string> name_a;
  std::optional<std::string> name_b;
};

std::vector<JoinedRecord> innerJoin(const Table& left, const Table& right);
std::vector<JoinedRecord> leftJoin(const Table& left, const Table& right);
std::vector<JoinedRecord> rightJoin(const Table& left, const Table& right);
std::vector<JoinedRecord> fullJoin(const Table& left, const Table& right);

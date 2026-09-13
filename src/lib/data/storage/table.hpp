#pragma once
#include "record.hpp"

#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

class Table {
public:
  explicit Table(std::string name);

  bool insert(int id, std::string name);
  bool remove(int id);
  void truncate();

  std::optional<Record> get(int id) const;
  std::vector<Record> getAll() const;
  size_t size() const;

  const std::unordered_map<int, std::string>& getRawData() const;

  std::shared_mutex& getMutex() const;

private:
  std::string name_;
  std::unordered_map<int, std::string> records_;
  mutable std::shared_mutex rw_mutex_;
};
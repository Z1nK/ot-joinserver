#pragma once

#include <data/storage/table.hpp>

#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

class DbEngine {
public:
  DbEngine() = default;

  bool createTable(const std::string& name);
  bool dropTable(const std::string& name);
  bool hasTable(const std::string& name) const;

  Table* getTable(const std::string& name);
  const Table* getTable(const std::string& name) const;

  std::vector<std::string> listTables() const;

private:
  mutable std::shared_mutex mutex_;
  std::unordered_map<std::string, std::unique_ptr<Table>> tables_;
};

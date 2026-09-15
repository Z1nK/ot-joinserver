#include "db_engine.hpp"

#include <mutex>

bool DbEngine::createTable(const std::string& name) {
  std::unique_lock lock(mutex_);
  auto [it, inserted] = tables_.try_emplace(name, std::make_unique<Table>(name));
  return inserted;
}

bool DbEngine::dropTable(const std::string& name) {
  std::unique_lock lock(mutex_);
  return tables_.erase(name) > 0;
}

bool DbEngine::hasTable(const std::string& name) const {
  std::shared_lock lock(mutex_);
  return tables_.contains(name);
}

Table* DbEngine::getTable(const std::string& name) {
  std::shared_lock lock(mutex_);
  auto it = tables_.find(name);
  return it != tables_.end() ? it->second.get() : nullptr;
}

const Table* DbEngine::getTable(const std::string& name) const {
  std::shared_lock lock(mutex_);
  auto it = tables_.find(name);
  return it != tables_.end() ? it->second.get() : nullptr;
}

std::vector<std::string> DbEngine::listTables() const {
  std::shared_lock lock(mutex_);
  std::vector<std::string> names;
  names.reserve(tables_.size());
  for (const auto& [name, table] : tables_) {
    names.push_back(name);
  }
  return names;
}

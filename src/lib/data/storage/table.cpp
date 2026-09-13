#include "table.hpp"

#include <mutex>

Table::Table(std::string name) : name_(std::move(name)) {}

bool Table::insert(int id, std::string name) {
  std::unique_lock lock(rw_mutex_);
  auto [it, inserted] = records_.emplace(id, std::move(name));
  return inserted;
}

bool Table::remove(int id) {
  std::unique_lock lock(rw_mutex_);
  return records_.erase(id) > 0;
}

void Table::truncate() {
  std::unique_lock lock(rw_mutex_);
  records_.clear();
}

std::optional<Record> Table::get(int id) const {
  std::shared_lock lock(rw_mutex_);
  auto it = records_.find(id);
  if (it != records_.end()) {
    return Record{it->first, it->second};
  }
  return std::nullopt;
}

std::vector<Record> Table::getAll() const {
  std::shared_lock lock(rw_mutex_);
  std::vector<Record> result;
  result.reserve(records_.size());
  for (const auto& [id, name] : records_) {
    result.push_back({id, name});
  }
  return result;
}

const std::unordered_map<int, std::string>& Table::getRawData() const {
  return records_;
}

std::shared_mutex& Table::getMutex() const {
  return rw_mutex_;
}

size_t Table::size() const {
  std::shared_lock lock(rw_mutex_);
  return records_.size();
}
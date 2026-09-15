#pragma once

#include <data/join/join.hpp>
#include <data/storage/record.hpp>

#include <string>
#include <string_view>
#include <vector>

std::string formatOk();
std::string formatError(std::string_view message);
std::string formatRows(const std::vector<JoinedRecord>& rows);
std::string formatRows(const std::vector<Record>& rows);

#include <network/protocol/parser.hpp>
#include <network/protocol/response.hpp>

#include <gtest/gtest.h>

TEST(ParseCommand, ParsesInsert) {
  ParseResult result = parseCommand("INSERT A 1 lean");
  ASSERT_TRUE(result.command.has_value());
  const auto* insert = std::get_if<InsertCommand>(&*result.command);
  ASSERT_NE(insert, nullptr);
  EXPECT_EQ(insert->table, "A");
  EXPECT_EQ(insert->id, 1);
  EXPECT_EQ(insert->name, "lean");
}

TEST(ParseCommand, RejectsInsertWithNonIntegerId) {
  ParseResult result = parseCommand("INSERT A x lean");
  EXPECT_FALSE(result.command.has_value());
  EXPECT_FALSE(result.error.empty());
}

TEST(ParseCommand, RejectsInsertWithWrongArity) {
  ParseResult result = parseCommand("INSERT A 1");
  EXPECT_FALSE(result.command.has_value());
}

TEST(ParseCommand, ParsesTruncate) {
  ParseResult result = parseCommand("TRUNCATE A");
  ASSERT_TRUE(result.command.has_value());
  const auto* truncate = std::get_if<TruncateCommand>(&*result.command);
  ASSERT_NE(truncate, nullptr);
  EXPECT_EQ(truncate->table, "A");
}

TEST(ParseCommand, ParsesCreateTable) {
  ParseResult result = parseCommand("CREATE_TABLE B");
  ASSERT_TRUE(result.command.has_value());
  const auto* createTable = std::get_if<CreateTableCommand>(&*result.command);
  ASSERT_NE(createTable, nullptr);
  EXPECT_EQ(createTable->table, "B");
}

TEST(ParseCommand, ParsesPrintTable) {
  ParseResult result = parseCommand("PRINT_TABLE A");
  ASSERT_TRUE(result.command.has_value());
  const auto* printTable = std::get_if<PrintTableCommand>(&*result.command);
  ASSERT_NE(printTable, nullptr);
  EXPECT_EQ(printTable->table, "A");
}

TEST(ParseCommand, RejectsPrintTableWithWrongArity) {
  ParseResult result = parseCommand("PRINT_TABLE");
  EXPECT_FALSE(result.command.has_value());
}

TEST(ParseCommand, ParsesNoArgCommands) {
  EXPECT_TRUE(std::holds_alternative<IntersectionCommand>(*parseCommand("INTERSECTION").command));
  EXPECT_TRUE(std::holds_alternative<SymmetricDifferenceCommand>(*parseCommand("SYMMETRIC_DIFFERENCE").command));
  EXPECT_TRUE(std::holds_alternative<LeftJoinCommand>(*parseCommand("LEFT_JOIN").command));
  EXPECT_TRUE(std::holds_alternative<RightJoinCommand>(*parseCommand("RIGHT_JOIN").command));
  EXPECT_TRUE(std::holds_alternative<FullJoinCommand>(*parseCommand("FULL_JOIN").command));
}

TEST(ParseCommand, RejectsNoArgCommandWithTrailingTokens) {
  ParseResult result = parseCommand("LEFT_JOIN extra");
  EXPECT_FALSE(result.command.has_value());
}

TEST(ParseCommand, RejectsUnknownCommand) {
  ParseResult result = parseCommand("DROP_TABLE A");
  EXPECT_FALSE(result.command.has_value());
  EXPECT_FALSE(result.error.empty());
}

TEST(ParseCommand, RejectsEmptyLine) {
  ParseResult result = parseCommand("");
  EXPECT_FALSE(result.command.has_value());
}

TEST(FormatResponse, FormatsOk) { EXPECT_EQ(formatOk(), "OK\n\n"); }

TEST(FormatResponse, FormatsError) { EXPECT_EQ(formatError("bad thing"), "ERR bad thing\n\n"); }

TEST(FormatResponse, FormatsEmptyJoinRows) { EXPECT_EQ(formatRows(std::vector<JoinedRecord>{}), "OK\n\n"); }

TEST(FormatResponse, FormatsJoinRowsWithNulls) {
  std::vector<JoinedRecord> rows = {{1, std::optional<std::string>("a"), std::nullopt}};
  EXPECT_EQ(formatRows(rows), "1,a,NULL\nOK\n\n");
}

TEST(FormatResponse, FormatsEmptyTableRows) { EXPECT_EQ(formatRows(std::vector<Record>{}), "OK\n\n"); }

TEST(FormatResponse, FormatsTableRows) {
  std::vector<Record> rows = {{1, "lean"}, {2, "sweater"}};
  EXPECT_EQ(formatRows(rows), "1,lean\n2,sweater\nOK\n\n");
}

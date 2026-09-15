#include <network/command-handler/command_handler.hpp>

#include <gtest/gtest.h>

namespace {

class CommandHandlerTest : public ::testing::Test {
protected:
  DbEngine engine;
  CommandHandler handler{engine};
};

}  // namespace

TEST_F(CommandHandlerTest, CreateTableSucceedsOnce) {
  EXPECT_EQ(handler.handle(CreateTableCommand{"A"}), "OK\n\n");
  EXPECT_EQ(handler.handle(CreateTableCommand{"A"}).substr(0, 3), "ERR");
}

TEST_F(CommandHandlerTest, InsertRequiresExistingTable) {
  EXPECT_EQ(handler.handle(InsertCommand{"A", 1, "lean"}).substr(0, 3), "ERR");
  handler.handle(CreateTableCommand{"A"});
  EXPECT_EQ(handler.handle(InsertCommand{"A", 1, "lean"}), "OK\n\n");
}

TEST_F(CommandHandlerTest, InsertRejectsDuplicateId) {
  handler.handle(CreateTableCommand{"A"});
  handler.handle(InsertCommand{"A", 1, "lean"});
  EXPECT_EQ(handler.handle(InsertCommand{"A", 1, "other"}).substr(0, 3), "ERR");
}

TEST_F(CommandHandlerTest, TruncateClearsTable) {
  handler.handle(CreateTableCommand{"A"});
  handler.handle(InsertCommand{"A", 1, "lean"});
  EXPECT_EQ(handler.handle(TruncateCommand{"A"}), "OK\n\n");
  EXPECT_EQ(handler.handle(InsertCommand{"A", 1, "lean"}), "OK\n\n");
}

TEST_F(CommandHandlerTest, JoinRequiresBothTables) {
  EXPECT_EQ(handler.handle(LeftJoinCommand{}).substr(0, 3), "ERR");
  handler.handle(CreateTableCommand{"A"});
  EXPECT_EQ(handler.handle(LeftJoinCommand{}).substr(0, 3), "ERR");
  handler.handle(CreateTableCommand{"B"});
  EXPECT_EQ(handler.handle(LeftJoinCommand{}), "OK\n\n");
}

TEST_F(CommandHandlerTest, IntersectionMapsToInnerJoin) {
  handler.handle(CreateTableCommand{"A"});
  handler.handle(CreateTableCommand{"B"});
  handler.handle(InsertCommand{"A", 1, "lean"});
  handler.handle(InsertCommand{"B", 1, "harry"});
  EXPECT_EQ(handler.handle(IntersectionCommand{}), "1,lean,harry\nOK\n\n");
}

TEST_F(CommandHandlerTest, SymmetricDifferenceExcludesMatchingIds) {
  handler.handle(CreateTableCommand{"A"});
  handler.handle(CreateTableCommand{"B"});
  handler.handle(InsertCommand{"A", 1, "lean"});
  handler.handle(InsertCommand{"A", 2, "sweater"});
  handler.handle(InsertCommand{"B", 2, "harry"});
  handler.handle(InsertCommand{"B", 3, "frank"});
  EXPECT_EQ(handler.handle(SymmetricDifferenceCommand{}), "1,lean,\n3,,frank\nOK\n\n");
}

TEST_F(CommandHandlerTest, FullJoinIncludesUnmatchedFromBothSides) {
  handler.handle(CreateTableCommand{"A"});
  handler.handle(CreateTableCommand{"B"});
  handler.handle(InsertCommand{"A", 1, "lean"});
  handler.handle(InsertCommand{"B", 2, "harry"});
  EXPECT_EQ(handler.handle(FullJoinCommand{}), "1,lean,\n2,,harry\nOK\n\n");
}

TEST_F(CommandHandlerTest, RightJoinKeepsAllOfB) {
  handler.handle(CreateTableCommand{"A"});
  handler.handle(CreateTableCommand{"B"});
  handler.handle(InsertCommand{"B", 1, "harry"});
  EXPECT_EQ(handler.handle(RightJoinCommand{}), "1,,harry\nOK\n\n");
}

TEST_F(CommandHandlerTest, PrintTableRequiresExistingTable) {
  EXPECT_EQ(handler.handle(PrintTableCommand{"A"}).substr(0, 3), "ERR");
}

TEST_F(CommandHandlerTest, PrintTableListsRowsSortedById) {
  handler.handle(CreateTableCommand{"A"});
  handler.handle(InsertCommand{"A", 2, "sweater"});
  handler.handle(InsertCommand{"A", 1, "lean"});
  EXPECT_EQ(handler.handle(PrintTableCommand{"A"}), "1,lean\n2,sweater\nOK\n\n");
}

TEST_F(CommandHandlerTest, PrintTableOnEmptyTable) {
  handler.handle(CreateTableCommand{"A"});
  EXPECT_EQ(handler.handle(PrintTableCommand{"A"}), "OK\n\n");
}

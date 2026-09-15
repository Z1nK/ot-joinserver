find_package(GTest REQUIRED)

include(GoogleTest)

add_executable(protocol_tests protocol_tests.cpp)

target_link_libraries(protocol_tests PRIVATE
  network::protocol-lib-static
  GTest::GTest
  GTest::Main
)

gtest_discover_tests(protocol_tests)

add_executable(command_handler_tests command_handler_tests.cpp)

target_link_libraries(command_handler_tests PRIVATE
  network::command-handler-lib-static
  GTest::GTest
  GTest::Main
)

gtest_discover_tests(command_handler_tests)

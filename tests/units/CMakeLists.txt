find_package(GTest REQUIRED)

add_executable(hash_tests hash_tests.cpp)


target_link_libraries(hash_tests PRIVATE
  bayan::hash-methods
  GTest::GTest
  GTest::Main
  Boost::headers
)

include(GoogleTest)
gtest_discover_tests(hash_tests)

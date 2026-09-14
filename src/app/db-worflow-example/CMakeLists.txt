add_executable(db-workflow-example)

target_compile_features(db-workflow-example PUBLIC cxx_std_20)

target_sources(db-workflow-example PRIVATE main.cpp)

target_link_libraries(db-workflow-example PRIVATE
    data::dbengine-lib-static
    data::join-lib-static
)

if (MSVC)
    target_compile_options(db-workflow-example PRIVATE /W4)
else()
    target_compile_options(db-workflow-example PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

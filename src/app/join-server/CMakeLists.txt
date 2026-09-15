add_executable(join_server)

target_compile_features(join_server PUBLIC cxx_std_20)

target_sources(join_server PRIVATE main.cpp)

target_link_libraries(join_server PRIVATE
    network::server-lib-static
    network::command-handler-lib-static
)

if (MSVC)
    target_compile_options(join_server PRIVATE /W4)
else()
    target_compile_options(join_server PRIVATE -Wall -Wextra -pedantic -Werror)
endif()


# Installation

install(TARGETS join_server RUNTIME DESTINATION bin)
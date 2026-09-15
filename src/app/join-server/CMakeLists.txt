add_executable(joinserver)

target_compile_features(joinserver PUBLIC cxx_std_20)

target_sources(joinserver PRIVATE main.cpp)

target_link_libraries(joinserver PRIVATE
    network::server-lib-static
    network::command-handler-lib-static
)

if (MSVC)
    target_compile_options(joinserver PRIVATE /W4)
else()
    target_compile_options(joinserver PRIVATE -Wall -Wextra -pedantic -Werror)
endif()


# Installation

install(TARGETS joinserver RUNTIME DESTINATION bin)
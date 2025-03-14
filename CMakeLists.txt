cmake_minimum_required(VERSION 3.31)
project(DirSize)

set(CMAKE_CXX_STANDARD 23)

add_executable(DirSize src/main.cpp)

execute_process(
        COMMAND bash -c "git describe --always --dirty --tags --exact-match 2>/dev/null || (git describe --always --dirty --match='NOT A MATCH' | awk 1 ORS='';echo '-git') || echo unknown"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE DSversion
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(NOTICE "Building version ${DSversion}. Reconfigure project to change.")

target_compile_definitions(DirSize
        PRIVATE DSIZE_VERSION="${DSversion}"
)


if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    message("Enabling tests")
    enable_testing()
    add_subdirectory(test)
endif ()

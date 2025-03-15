cmake_minimum_required(VERSION 3.31)
project(DirSize)

set(CMAKE_CXX_STANDARD 23)

add_executable(DirSize src/main.cpp)

execute_process(
        COMMAND bash -c "git describe --always --dirty --tags --exact-match 2>/dev/null || (git describe --always --dirty --match='NOT A MATCH' 2>/dev/null | awk 1 ORS='' && echo '-git')"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE DSversion
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

option(DSIZE_ENABLE_TESTS "Enable Building Tests" OFF)

if (NOT "${DSversion}" STREQUAL "")
    message(NOTICE "Building version ${DSversion}. Reconfigure project to change.")
    target_compile_definitions(DirSize
            PRIVATE DSIZE_VERSION="${DSversion}"
    )
endif ()


if(DSIZE_ENABLE_TESTS AND CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)
endif ()

# Windows getopt support
if (WIN32)
    include(FetchContent)
    fetchcontent_declare(
            wingetopt
            GIT_REPOSITORY https://github.com/alex85k/wingetopt
            GIT_TAG master
    )

    fetchcontent_makeavailable(
            wingetopt
    )
    target_link_libraries(DirSize PRIVATE wingetopt)
    target_include_directories(DirSize PRIVATE ${wingetopt_SOURCE_DIR}/src/)
endif ()
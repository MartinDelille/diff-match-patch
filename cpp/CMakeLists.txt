cmake_minimum_required(VERSION 3.16...3.29 FATAL_ERROR)

# Meta information about the project
set(META_PROJECT_NAME        "diff-match-patch")
set(META_PROJECT_DESCRIPTION "diff-match-patch")
set(META_VERSION_MAJOR       "0")
set(META_VERSION_MINOR       "0")
set(META_VERSION_PATCH       "3")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")

# Define the project itself
project(${META_PROJECT_NAME}
    VERSION ${META_VERSION}
    DESCRIPTION ${META_PROJECT_DESCRIPTION}
    LANGUAGES C CXX
)

find_package(Qt5 REQUIRED COMPONENTS Core)

# Target name
set(target diff-match-patch)

# Define to build a library
add_library(${target}
    diff_match_patch.h
    diff_match_patch.cpp
    diff_match_patch_test.h
    diff_match_patch_test.cpp
)

# Create namespaced alias
add_library(${META_PROJECT_NAME}::${target} ALIAS ${target})

# Export library for downstream projects
export(TARGETS ${target} NAMESPACE ${META_PROJECT_NAME}:: FILE ${PROJECT_BINARY_DIR}/cmake/${target}/${target}-export.cmake)

# Project options
set_target_properties(${target}
    PROPERTIES
        CXX_STANDARD_REQUIRED ON
)

# Include directories
target_include_directories(${target}
    PRIVATE
        ${QWT_INCLUDE_DIRS}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Libraries
target_link_libraries(${target}
    PUBLIC
        Qt5::Core
)

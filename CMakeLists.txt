cmake_minimum_required(VERSION 3.9)
project(main-executable C)

if(NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/URCL-CC)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/URCL-CC)
option(BUILD_SHARED_LIBS "" off)

# add_subdirectory(extern)

set(SOURCE_FILES
    source/main.c
)

add_executable(main-executable ${SOURCE_FILES})

target_compile_features(main-executable PUBLIC c_std_99)
set_target_properties(main-executable PROPERTIES OUTPUT_NAME "urcl-cc")

target_include_directories(main-executable PUBLIC "extern")
target_include_directories(main-executable PUBLIC "source") # to allow paths relative to project root

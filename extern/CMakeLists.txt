include(FetchContent)

# Include fmt
FetchContent_Declare(
    fmt
    GIT_REPOSITORY  https://github.com/fmtlib/fmt
    GIT_TAG         7.1.3
)
FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    FetchContent_Populate(fmt)

    # Disable tests
    set(FMT_TEST OFF)

    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif()
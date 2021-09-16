include(FetchContent)

# Include zlib-ng
set(ZLIB_ENABLE_TESTS OFF)
set(WITH_NATIVE_INSTRUCTIONS ON)

FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng
        GIT_TAG        2.0.5
)

FetchContent_MakeAvailable(zlib)

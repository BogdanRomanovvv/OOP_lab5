cmake_minimum_required(VERSION 3.14)
project(OOP_lab_5 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основная библиотека
add_library(lab5_lib INTERFACE)
target_include_directories(lab5_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Исполняемый файл
add_executable(lab5_app src/main.cpp)
target_link_libraries(lab5_app PRIVATE lab5_lib)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Тесты
add_executable(lab5_tests tests/test_memory_resource.cpp tests/test_dynamic_array.cpp)
target_link_libraries(lab5_tests PRIVATE lab5_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(lab5_tests)

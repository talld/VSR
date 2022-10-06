project(VSR_TESTS)
cmake_minimum_required(VERSION 3.19)
set(CMAKE_C_STANDARD 99)

add_executable(${PROJECT_NAME}
        tests.c)


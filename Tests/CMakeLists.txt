project(VSR_TESTS)
cmake_minimum_required(VERSION 3.19)
set(CMAKE_C_STANDARD 99)

add_executable(${PROJECT_NAME}
        tests.c)

target_compile_options(${PROJECT_NAME} PRIVATE
        -fno-omit-frame-pointer
        -fsanitize=address
        -fsanitize=undefined
        -fsanitize=leak
        -Wall
        -Wno-unused-label
        -Werror)

target_link_options(${PROJECT_NAME} PRIVATE
        -fno-omit-frame-pointer
        -fsanitize=address
        -fsanitize=undefined
        -fsanitize=leak)

target_link_libraries(${PROJECT_NAME} VSR)
target_include_directories(${PROJECT_NAME} PUBLIC ${VSR_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ../VSR/Inc)
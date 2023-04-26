project(VSR_EXAMPLES)
cmake_minimum_required(VERSION 3.19)
set(CMAKE_C_STANDARD 99)

add_executable(${PROJECT_NAME}
        helpers.c
        demos.c
        )

target_compile_options(${PROJECT_NAME} PRIVATE
        -fno-omit-frame-pointer
        -Wall
        )

target_link_options(${PROJECT_NAME} PRIVATE
        -fno-omit-frame-pointer
        )

add_subdirectory(External/cglm)
target_include_directories(${PROJECT_NAME} PUBLIC External/tinyobjloader-c/)

target_link_libraries(${PROJECT_NAME} PRIVATE VSR cglm)
target_include_directories(${PROJECT_NAME} PUBLIC ${VSR_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ../VSR/Inc)



project(VSR)
cmake_minimum_required(VERSION 3.19)
set(CMAKE_C_STANDARD 99)

add_library(${PROJECT_NAME}
        Src/VSR.c)

find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
target_include_directories(${PROJECT_NAME} PRIVATE ${VULKAN_INCLUDE_DIRS})

find_package(SDL2 REQUIRED FATAL_ERROR)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2)
target_include_directories(${PROJECT_NAME} PRIVATE ${VULKAN_INCLUDE_DIRS})

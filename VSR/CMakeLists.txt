project(VSR)
cmake_minimum_required(VERSION 3.19)
set(CMAKE_C_STANDARD 99)

set(VSR_INC
        Src)

set(VSR_PCH
        Inc/VSR.h)

set(VSR_SRC
        ${VSR_PCH}
        Src/VSR_error.c
        Src/Renderer.c)

add_library(${PROJECT_NAME} ${VSR_SRC})

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

target_precompile_headers(${PROJECT_NAME} PRIVATE ${VSR_PCH})
target_include_directories(${PROJECT_NAME} PRIVATE ${VSR_INC})

find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
target_include_directories(${PROJECT_NAME} PRIVATE ${VULKAN_INCLUDE_DIRS})

find_package(SDL2 REQUIRED FATAL_ERROR)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL_INCLUDE_DIRS})

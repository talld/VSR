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

        # Rendering
        Src/Renderer.c
        Src/Renderer_Instance.c
        Src/Renderer_PhysicalDevice.c
        Src/Renderer_LogicalDevice.c
        Src/Renderer_DeviceQueues.c
        Src/Renderer_Surface.c
        Src/Renderer_Swapchain.c
        Src/Renderer_ImageView.c
        Src/VSR_Shader.c
        Src/Renderer_RenderPass.c
        Src/Renderer_GraphicsPipeline.c
        Src/ComputePipeline.c
        )

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
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

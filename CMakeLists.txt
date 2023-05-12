project(VSR)
cmake_minimum_required(VERSION 3.19)
set(CMAKE_C_STANDARD 99)

set(VSR_INC
        Inc)

set(VSR_PCH
        Inc/VSR.h)

set(VSR_SRC
        ${VSR_PCH}
        Src/VSR_error.c

        ### Rendering ###

        # Renderer
        Src/VSR_Renderer.c
        Src/Renderer_Instance.c
        Src/Renderer_PhysicalDevice.c
        Src/Renderer_LogicalDevice.c
        Src/Renderer_Memory.c
        Src/Renderer_DeviceQueues.c
        Src/Renderer_Surface.c
        Src/Renderer_Swapchain.c
        Src/Renderer_CommandPool.c
        Src/Renderer_DescriptorPool.c


        # GraphicsPipeline
        Src/VSR_GraphicsPipeline.c
        Src/VSR_Shader.c
        Src/VSR_RenderPass.c
        Src/GraphicsPipeline_GraphicsPipeline.c
        Src/VSR_Framebuffer.c
        Src/VSR_Image.c
        Src/VSR_Sampler.c
        Src/VSR_Mesh.c
        Src/VSR_Model.c
        Src/VSR_Mat4.c
        )

add_library(${PROJECT_NAME} STATIC ${VSR_SRC})

target_compile_options(${PROJECT_NAME} PRIVATE
        -fno-omit-frame-pointer
        -Wall
        )

target_link_options(${PROJECT_NAME} PRIVATE
        -fno-omit-frame-pointer

        -fsanitize=address   -static-libasan
        -fsanitize=undefined -static-libubsan
        -fsanitize=leak      -static-liblsan
        )

target_include_directories(${PROJECT_NAME} PRIVATE ${VSR_INC})

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
target_include_directories(${PROJECT_NAME} PRIVATE ${VULKAN_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

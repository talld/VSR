#ifndef VSR_SUITE_RENDERER_H
#define VSR_SUITE_RENDERER_H

#include <vulkan/vulkan.h>

#include "Renderer_Instance.h"
#include "Renderer_Surface.h"
#include "Renderer_PhysicalDevice.h"
#include "Renderer_LogicalDevice.h"
#include "Renderer_DeviceQueues.h"
#include "Renderer_Swapchain.h"
#include "VSR_Shader.h"
#include "Renderer_RenderPass.h"
#include "Renderer_GraphicsPipeline.h"
#include "Renderer_Framebuffer.h"
#include "Renderer_CommandPool.h"


typedef struct VSR_RendererCreateInfoSubStructs VSR_RendererCreateInfoSubStructs;
struct VSR_RendererCreateInfoSubStructs
{
	VSR_InstanceCreateInfo          instanceCreateInfo;
	VSR_SurfaceCreateInfo           surfaceCreateInfo;
	VSR_PhysicalDeviceCreateInfo    physicalDeviceCreateInfo;
	VSR_LogicalDeviceCreateInfo     logicalDeviceCreateInfo;
	VSR_DeviceQueuesCreateInfo      deviceQueuesCreateInfo;
	VSR_SwapchainCreateInfo         swapchainCreateInfo;
	VSR_RenderPassCreateInfo        renderPassCreateInfo;
	VSR_GraphicPipelineCreateInfo   graphicsPipelineCreateInfo;
	VSR_FramebufferCreateInfo       framebufferCreateInfo;
	VSR_CommandPoolCreateInfo       commandPoolCreateInfo;
};


typedef struct VSR_RendererCreateInfo VSR_RendererCreateInfo;
struct VSR_RendererCreateInfo
{
	SDL_Window*   SDLWindow;

	SDL_bool   geometryShaderRequested;
	SDL_bool   tessellationShaderRequested;

	VSR_RendererCreateInfoSubStructs*  subStructs;
};

typedef struct VSR_RendererSubStructs VSR_RendererSubStructs;
struct VSR_RendererSubStructs
{
	VSR_Instance         instance;
	VSR_Surface          surface;
	VSR_PhysicalDevice   physicalDevice;
	VSR_DeviceQueues     deviceQueues;
	VSR_LogicalDevice    logicalDevice;
	VSR_Swapchain        swapchain;
	VSR_Shader*          vertexShader;
	VSR_Shader*          fragmentShader;
	VSR_RenderPass       renderPass;
	VSR_GraphicPipeline  graphicPipeline;
	VSR_Framebuffer      framebuffer;
	VSR_CommandPool      commandPool;
	VkSemaphore          imageCanBeWritten;
	VkSemaphore          imageCanBeRead;
};

typedef struct VSR_Renderer VSR_Renderer;
struct VSR_Renderer
{
	SDL_Window* SDLWindow;
	VSR_RendererSubStructs* subStructs;
};

// Temp allocate function
// TODO: replace with function in VSR_mem.h ( or whatever )
static inline const VkAllocationCallbacks*
VSR_GetAllocator()
{
	return NULL;
}

#endif // VSR_SUITE_RENDERER_H

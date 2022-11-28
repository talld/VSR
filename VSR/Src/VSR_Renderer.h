#ifndef VSR_RENDERER_H
#define VSR_RENDERER_H

#include <vulkan/vulkan.h>
#include "VSR_GraphicsPipeline.h"

#include "Renderer_Instance.h"
#include "Renderer_Surface.h"
#include "Renderer_PhysicalDevice.h"
#include "Renderer_LogicalDevice.h"
#include "Renderer_DeviceQueues.h"
#include "Renderer_Swapchain.h"


typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs
{
	Renderer_InstanceCreateInfo         instanceCreateInfo;
	Renderer_SurfaceCreateInfo          surfaceCreateInfo;
	Renderer_PhysicalDeviceCreateInfo   physicalDeviceCreateInfo;
	Renderer_LogicalDeviceCreateInfo    logicalDeviceCreateInfo;
	Renderer_DeviceQueuesCreateInfo     deviceQueuesCreateInfo;
	Renderer_SwapchainCreateInfo        swapchainCreateInfo;
};


typedef struct VSR_RendererCreateInfo VSR_RendererCreateInfo;
struct VSR_RendererCreateInfo
{
	SDL_Window*   SDLWindow;

	SDL_bool   geometryShaderRequested;
	SDL_bool   tessellationShaderRequested;

	Renderer_CreateInfoSubStructs*  subStructs;
};

typedef struct Renderer_SubStructs Renderer_SubStructs;
struct Renderer_SubStructs
{
	Renderer_Instance       instance;
	Renderer_Surface        surface;
	Renderer_PhysicalDevice physicalDevice;
	Renderer_DeviceQueues   deviceQueues;
	Renderer_LogicalDevice  logicalDevice;
	Renderer_Swapchain      swapchain;

	VSR_GraphicsPipeline*   pipeline;

	VkSemaphore*            imageCanBeWritten;
	VkSemaphore*            imageCanBeRead;
	VkFence*                imageFinished;

	size_t                  currentFrame;
};

typedef struct VSR_Renderer VSR_Renderer;
struct VSR_Renderer
{
	SDL_Window         * SDLWindow;
	Renderer_SubStructs* subStructs;
};

// Temp allocate function
// TODO: replace with function in VSR_mem.h ( or whatever )
static inline const VkAllocationCallbacks*
VSR_GetAllocator()
{
	return NULL;
}

#endif // VSR_RENDERER_H

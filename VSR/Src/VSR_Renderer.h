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
#include "Renderer_Memory.h"


typedef enum Renderer_ResourceType Renderer_ResourceType;
enum Renderer_ResourceType
{
	RESOURCE_TYPE_VERTEX,
	RESOURCE_TYPE_INDEX,
	RESOURCE_TYPE_UV,
};

// struct for holding (sub-allocated) data used by a model
typedef struct Renderer_ModelBuffer Renderer_ModelBuffer;
struct Renderer_ModelBuffer
{
	VSR_Mesh* pModel;
	Renderer_ResourceType resourceType; // not needed but good for sanity checks
	size_t offset;
	size_t len;
};

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
	/// render substructs ///
	Renderer_Instance       instance;
	Renderer_Surface        surface;
	Renderer_PhysicalDevice physicalDevice;
	Renderer_DeviceQueues   deviceQueues;
	Renderer_LogicalDevice  logicalDevice;
	Renderer_Swapchain      swapchain;

	/// pipeline ( subject to change at runtime! ) ///
	VSR_GraphicsPipeline*   pipeline;

	/// render sync ///
	VkSemaphore*            imageCanBeWritten;
	VkSemaphore*            imageCanBeRead;
	VkFence*                imageFinished;

	/// memory ///
	Renderer_Memory VUIStagingBuffer;
	Renderer_Memory VUIGPUBuffer;


	uint32_t                imageIndex;
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

#ifndef VSR_RENDERER_H
#define VSR_RENDERER_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

#include "Renderer_Instance.h"
#include "Renderer_Surface.h"
#include "Renderer_PhysicalDevice.h"
#include "Renderer_LogicalDevice.h"
#include "Renderer_DeviceQueues.h"
#include "Renderer_Swapchain.h"
#include "Renderer_Memory.h"

#include "VSR_GraphicsPipeline.h"
#include "VSR_Model.h"
#include "VSR_Mesh.h"
#include "VSR_Sampler.h"


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

enum {kMaxSupportedStorageBuffers = 4};
typedef struct VSR_RendererCreateInfo VSR_RendererCreateInfo;
struct VSR_RendererCreateInfo
{
	SDL_Window* SDLWindow;

	SDL_bool geometryShaderRequested;
	SDL_bool tessellationShaderRequested;

	size_t cmdBuffersPerPool;
	size_t texturePoolSize;

	size_t extraDescriptorSizes[kMaxSupportedStorageBuffers];
	size_t extraDescriptorCount;

	Renderer_InstanceCreateInfo       instanceCreateInfo;
	Renderer_SurfaceCreateInfo        surfaceCreateInfo;
	Renderer_PhysicalDeviceCreateInfo physicalDeviceCreateInfo;
	Renderer_LogicalDeviceCreateInfo  logicalDeviceCreateInfo;
	Renderer_DeviceQueuesCreateInfo   deviceQueuesCreateInfo;
	Renderer_SwapchainCreateInfo      swapchainCreateInfo;

	Renderer_CommandPoolCreateInfo    commandPoolCreateInfo;
	Renderer_DescriptorPoolCreateInfo descriptorPoolCreateInfo;
};

typedef struct VSR_Renderer VSR_Renderer;
struct VSR_Renderer
{
	SDL_Window* SDLWindow;


	Renderer_Instance       instance;
	Renderer_Surface        surface;
	Renderer_PhysicalDevice physicalDevice;
	Renderer_DeviceQueues   deviceQueues;
	Renderer_LogicalDevice  logicalDevice;
	Renderer_Swapchain      swapchain;

	Renderer_DescriptorPool descriptorPool;
	Renderer_CommandPool    commandPool;

	/// pipeline ( subject to change at runtime! ) ///
	VSR_GraphicsPipeline* pipeline;

	/// render sync ///
	VkSemaphore* imageCanBeWritten;
	VkSemaphore* imageCanBeRead;
	VkFence*     imageFinished;

	/// memory ///
	VSR_PushConstants pushConstantsVertex;
	VSR_PushConstants pushConstantsFragment;

	size_t texturePoolSize;

	size_t*               extraDescriptorSizes;
	Renderer_MemoryAlloc* extraDescriptorAllocs[kMaxSupportedStorageBuffers];
	size_t                extraDescriptorCount;

	VSR_Sampler* defaultSampler;

	/// Vertex UV Index ///
	Renderer_Memory* VIStagingBuffer;
	Renderer_Memory* VIGPUBuffer;
	Renderer_Memory* scratchBuffer;

	/// Uniform Storage Descriptor ///
	Renderer_Memory* USDStagingBuffer;
	Renderer_Memory* USDGPUBuffer;

	uint32_t imageIndex;
	size_t currentFrame;
};

// Temp allocate function
// TODO: replace with function in VSR_mem.h ( or whatever )
static inline const VkAllocationCallbacks*
VSR_GetAllocator()
{
	return NULL;
}

#endif // VSR_RENDERER_H

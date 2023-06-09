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


typedef struct QueuedRenderCommand QueuedRenderCommand;
struct QueuedRenderCommand
{
	VSR_Model* model;

	Renderer_MemoryAlloc* instanceDataAlloc;
	size_t instanceDataCount;

	VkCommandBuffer cBuff;
	VkSemaphore signalSemaphore;

	QueuedRenderCommand* next;
};

enum {kMaxSupportedStorageBuffers = 4};
typedef struct VSR_RendererCreateInfo VSR_RendererCreateInfo;
struct VSR_RendererCreateInfo
{
	SDL_Window* SDLWindow;

	SDL_bool geometryShaderRequested;
	SDL_bool tessellationShaderRequested;

	size_t vertexStagingBufferSize;
	size_t perModelVertexGPUBufferSize;
	size_t DescriptorSamplerStagingBufferSize;
	size_t DescriptorSamplerGPUBufferSize;

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
	size_t                  swapchainImageCount;

	uint32_t imageIndex;
	size_t currentFrame;

	/// render sync ///
	VkSemaphore*           imageCanBeWritten;
	VkSemaphore*           imageCanBeRead;
	VSR_GenerationalFence* imageFinished;
	size_t*                generationAcquired;

	Renderer_DescriptorPool descriptorPool;
	Renderer_CommandPool    commandPool;

	/// pipeline ( subject to change at runtime! ) ///
	VSR_GraphicsPipeline* pipeline;

	/// memory ///
	size_t texturePoolSize;

	size_t*               extraDescriptorSizes;
	Renderer_MemoryAlloc* extraDescriptorAllocs[kMaxSupportedStorageBuffers];
	size_t                extraDescriptorCount;

	VSR_Sampler* defaultSampler;

	/// Vertex UV Index ///
	Renderer_Memory* vertexStagingBuffer;
	Renderer_Memory* perModelVertexGPUBuffer;

	/// Uniform Storage Descriptor ///
	Renderer_Memory* USDStagingBuffer;
	Renderer_Memory* USDGPUBuffer;

	QueuedRenderCommand* activeRenderCommands;
	QueuedRenderCommand* savedRenderCommands;
};

// Temp allocate function
// TODO: replace with function in VSR_mem.h ( or whatever )
static inline const VkAllocationCallbacks*
VSR_GetAllocator()
{
	return NULL;
}

VSR_Framebuffer**
Renderer_GetSwapchainFrames(
	VSR_Renderer* renderer);

#endif // VSR_RENDERER_H

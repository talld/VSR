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

enum {kMaxSupportedStorageBuffers = 4};
typedef struct VSR_RendererCreateInfo VSR_RendererCreateInfo;
struct VSR_RendererCreateInfo
{
	SDL_Window* SDLWindow;

	SDL_bool geometryShaderRequested;
	SDL_bool tessellationShaderRequested;

	size_t vertexStagingBufferSize;
	size_t perModelVertexGPUBufferSize;
	size_t perInstanceVertexGPUBufferSize;

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
	Renderer_RenderPass     renderPass;

	/// render images ///
	VSR_Framebuffer** swapchainFrames;
	size_t           swapchainImageCount;

	VSR_Image*     depthImage;
	VSR_ImageView* depthView;

	uint32_t imageIndex;
	size_t currentFrame;


	/// render sync ///
	VkSemaphore* imageCanBeWritten;
	VkSemaphore* imageCanBeRead;
	VkFence*     imageFinished;

	Renderer_DescriptorPool descriptorPool;
	Renderer_CommandPool    commandPool;

	/// pipeline ( subject to change at runtime! ) ///
	VSR_GraphicsPipeline* pipeline;

	/// memory ///
	VSR_PushConstants pushConstantsVertex;
	VSR_PushConstants pushConstantsFragment;

	size_t texturePoolSize;

	uint64_t* modelSamplerMatrixArray;
	size_t    samplerMatrixArrayLength;
	size_t    matrixStartIndex;
	size_t    modelInstanceCount;

	size_t*               extraDescriptorSizes;
	Renderer_MemoryAlloc* extraDescriptorAllocs[kMaxSupportedStorageBuffers];
	size_t                extraDescriptorCount;

	VSR_Sampler* defaultSampler;

	/// Vertex UV Index ///
	Renderer_Memory* vertexStagingBuffer;

	Renderer_Memory* perModelVertexGPUBuffer;
	Renderer_Memory* perInstanceVertexGPUBuffer;

	/// Uniform Storage Descriptor ///
	Renderer_Memory* USDStagingBuffer;
	Renderer_Memory* USDGPUBuffer;
};

// Temp allocate function
// TODO: replace with function in VSR_mem.h ( or whatever )
static inline const VkAllocationCallbacks*
VSR_GetAllocator()
{
	return NULL;
}

#endif // VSR_RENDERER_H

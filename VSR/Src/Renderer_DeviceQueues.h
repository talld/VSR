#ifndef VSR_DEVICEQUEUES_H
#define VSR_DEVICEQUEUES_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

////////////////////////////////
/// Renderer pre-declaration ///
////////////////////////////////
typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs;


typedef struct Renderer_DeviceQueuesCreateInfo Renderer_DeviceQueuesCreateInfo;
struct Renderer_DeviceQueuesCreateInfo
{
	VkDeviceQueueCreateInfo   queueCreateInfoList[3];
	size_t                    transferQueueCreateInfoIndex;
	size_t                    graphicsQueueCreateInfoIndex;
	size_t                    computeQueueCreateInfoIndex;
};


typedef struct Renderer_DeviceQueues Renderer_DeviceQueues;
struct Renderer_DeviceQueues
{
	uint32_t   graphicsQueueIndex;
	uint32_t   computeQueueIndex;
	uint32_t   transferQueueIndex;

	uint32_t   graphicsQueueFamilyIndex;
	uint32_t   computeQueueFamilyIndex;
	uint32_t   transferQueueFamilyIndex;

	VkBool32   graphicsCanPresent;
	VkBool32   computeCanPresent;

	VkQueue    graphicsQueue;
	VkQueue    presentQueue;
	VkQueue    transferQueue;
	VkQueue    computeQueue;
};

SDL_bool
VSR_DeviceQueuesPopulateCreateInfo(
	VSR_RendererCreateInfo* rendererCreateInfo,
	Renderer_CreateInfoSubStructs* subStructs);

Renderer_DeviceQueues
VSR_DeviceQueuesSelectFromDevice(
	VkPhysicalDevice physicalDevice,
	VSR_Renderer* renderer
);

SDL_bool
VSR_DeviceQueuesCreate(
	VSR_Renderer* renderer,
	Renderer_CreateInfoSubStructs* subStructs);

#endif // VSR_DEVICEQUEUES_H

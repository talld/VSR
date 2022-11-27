#ifndef VSR_DEVICEQUEUES_H
#define VSR_DEVICEQUEUES_H

#include <vulkan/vulkan.h>


typedef struct VSR_DeviceQueuesCreateInfo VSR_DeviceQueuesCreateInfo;
struct VSR_DeviceQueuesCreateInfo
{
	VkDeviceQueueCreateInfo   queueCreateInfoList[3];
	size_t                    transferQueueCreateInfoIndex;
	size_t                    graphicsQueueCreateInfoIndex;
	size_t                    computeQueueCreateInfoIndex;
};


typedef struct VSR_DeviceQueues VSR_DeviceQueues;
struct VSR_DeviceQueues
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
	VSR_RendererCreateInfoSubStructs* subStructs);

VSR_DeviceQueues
VSR_DeviceQueuesSelectFromDevice(
	VkPhysicalDevice physicalDevice,
	VSR_Renderer* renderer
);

SDL_bool
VSR_DeviceQueuesCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs);

#endif // VSR_DEVICEQUEUES_H

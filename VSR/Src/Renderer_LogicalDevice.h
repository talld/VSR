#ifndef VSR_LOGICALDEVICE_H
#define VSR_LOGICALDEVICE_H

#include <vulkan/vulkan.h>


typedef struct VSR_LogicalDeviceCreateInfo VSR_LogicalDeviceCreateInfo;
struct VSR_LogicalDeviceCreateInfo
{
	VkDeviceCreateInfo   createInfo;
};


typedef struct VSR_LogicalDevice VSR_LogicalDevice;
struct VSR_LogicalDevice
{
	VkDevice   device;
};


SDL_bool
VSR_LogicalDevicePopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs);

SDL_bool
VSR_LogicalDeviceCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs);

void
VSR_LogicalDeviceDestroy(
	VSR_Renderer* renderer);

#endif // VSR_LOGICALDEVICE_H

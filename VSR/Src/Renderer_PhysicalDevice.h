#ifndef VSR_PHYSICALDEVICE_H
#define VSR_PHYSICALDEVICE_H

#include <vulkan/vulkan.h>

typedef struct VSR_PhysicalDeviceCreateInfo VSR_PhysicalDeviceCreateInfo;
struct VSR_PhysicalDeviceCreateInfo
{
	VkPhysicalDeviceFeatures2    physicalDeviceFeatures2;
};

typedef struct VSR_PhysicalDevice VSR_PhysicalDevice;
struct VSR_PhysicalDevice
{
	VkPhysicalDevice                     device;
	VkPhysicalDeviceProperties2          deviceProperties;
	VkPhysicalDeviceVulkan11Properties   deviceVulkan11Properties;
	VkPhysicalDeviceVulkan12Properties   deviceVulkan12Properties;
	VkPhysicalDeviceVulkan13Properties   deviceVulkan13Properties;
};

SDL_bool
VSR_PhysicalDeviceSelect(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* vkStructs);

#endif //VSR_PHYSICALDEVICE_H

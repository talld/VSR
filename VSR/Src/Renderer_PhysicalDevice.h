#ifndef VSR_PHYSICALDEVICE_H
#define VSR_PHYSICALDEVICE_H

#include <vulkan/vulkan.h>


////////////////////////////////
/// Renderer pre-declaration ///
////////////////////////////////
typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs;


typedef struct Renderer_PhysicalDeviceCreateInfo Renderer_PhysicalDeviceCreateInfo;
struct Renderer_PhysicalDeviceCreateInfo
{
	VkPhysicalDeviceFeatures2    physicalDeviceFeatures2;
};

typedef struct Renderer_PhysicalDevice Renderer_PhysicalDevice;
struct Renderer_PhysicalDevice
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
	Renderer_CreateInfoSubStructs* vkStructs);

#endif //VSR_PHYSICALDEVICE_H

#ifndef VSR_LOGICALDEVICE_H
#define VSR_LOGICALDEVICE_H

#include <vulkan/vulkan.h>


////////////////////////////////
/// Renderer pre-declaration ///
////////////////////////////////
typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs;


typedef struct Renderer_LogicalDeviceCreateInfo Renderer_LogicalDeviceCreateInfo;
struct Renderer_LogicalDeviceCreateInfo
{
	VkDeviceCreateInfo   createInfo;
};


typedef struct Renderer_LogicalDevice Renderer_LogicalDevice;
struct Renderer_LogicalDevice
{
	VkDevice   device;
};


SDL_bool
VSR_LogicalDevicePopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	Renderer_CreateInfoSubStructs* subStructs);

SDL_bool
VSR_LogicalDeviceCreate(
	VSR_Renderer* renderer,
	Renderer_CreateInfoSubStructs* subStructs);

void
VSR_LogicalDeviceDestroy(
	VSR_Renderer* renderer);

VkBuffer
VSR_LogicalDeviceCreateBuffer(
	VSR_Renderer* renderer,
	size_t size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties);

void
VSR_LogicalDeviceFreeBuffer(
	VSR_Renderer* renderer,
	VkBuffer buffer);

#endif // VSR_LOGICALDEVICE_H

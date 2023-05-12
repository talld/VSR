#ifndef VSR_LOGICALDEVICE_H
#define VSR_LOGICALDEVICE_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

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
	VSR_RendererCreateInfo* createInfo);

SDL_bool
VSR_LogicalDeviceCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo);

void
VSR_LogicalDeviceDestroy(
	VSR_Renderer* renderer);

VkDeviceMemory
VSR_LogicalDeviceGetMemory(
	VSR_Renderer* renderer,
	size_t size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer* buffer);

void
VSR_LogicalDeviceFreeMemory(
	VSR_Renderer* renderer,
	VkDeviceMemory memory);

#endif // VSR_LOGICALDEVICE_H

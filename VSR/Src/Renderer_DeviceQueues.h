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

enum {kMaxUniqueQFamilies = 4}; // one for each role

typedef struct Renderer_DeviceQueuesCreateInfo Renderer_DeviceQueuesCreateInfo;
struct Renderer_DeviceQueuesCreateInfo
{


};

enum {kGraphicsQueueIndex = 0,
	kComputeQueueIndex = 1,
	kTransferQueueIndex = 2,
	kPresentQueueIndex = 3,
}; // one for each role
typedef struct Renderer_DeviceQueues Renderer_DeviceQueues;
struct Renderer_DeviceQueues
{
	uint32_t uniqueQFamilies[kMaxUniqueQFamilies];
	uint32_t uniqueQIndex[kMaxUniqueQFamilies];
	float queuePriorities[kMaxUniqueQFamilies];
	size_t uniqueQFamilyCount;

	VkBool32 QCanPresent[kMaxUniqueQFamilies];
	uint32_t QFamilies[kMaxUniqueQFamilies];
	uint32_t QFamilyIndexes[kMaxUniqueQFamilies];

	VkQueue QList[kMaxUniqueQFamilies];
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

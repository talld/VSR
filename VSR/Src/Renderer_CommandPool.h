#ifndef VSR_COMMANDPOOL_H
#define VSR_COMMANDPOOL_H

#include "vulkan/vulkan.h"

typedef struct VSR_CommandPoolCreateInfo VSR_CommandPoolCreateInfo;
struct VSR_CommandPoolCreateInfo
{
	VkCommandPoolCreateInfo commandPoolCreateInfo;
};

typedef struct VSR_CommandPool VSR_CommandPool;
struct VSR_CommandPool
{
	VkCommandPool graphicsPool;
	VkCommandBuffer* commandBuffers;
};


SDL_bool
VSR_CommandPoolPopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs);


SDL_bool
VSR_CommandPoolCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs);

void
VSR_CommandPoolDestroy(
	VSR_Renderer* renderer
);

#endif //VSR_COMMANDPOOL_H

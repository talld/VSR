#ifndef VSR_COMMAND_POOL_H
#define VSR_COMMAND_POOL_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

////////////////////////////////
/// Renderer pre-declaration ///
////////////////////////////////
typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs;

////////////////////////////////////////
/// GraphicsPipeline pre-declaration ///
////////////////////////////////////////
typedef struct GraphicsPipeline_CreateInfoSubStructs GraphicsPipeline_CreateInfoSubStructs;
struct GraphicsPipeline_CreateInfoSubStructs;


typedef struct Renderer_CommandPoolCreateInfo Renderer_CommandPoolCreateInfo;
struct Renderer_CommandPoolCreateInfo
{
	VkCommandPoolCreateInfo commandPoolCreateInfo;
};

typedef struct Renderer_CommandPool Renderer_CommandPool;
struct Renderer_CommandPool
{
	VkCommandPool graphicsPool;
	VkCommandPool transferPool;
};


SDL_bool
Renderer_CommandPoolPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo);


SDL_bool
Renderer_CommandPoolCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo);

void
Renderer_CommandPoolDestroy(
	VSR_Renderer* renderer);

VkCommandBuffer
Renderer_CommandPoolAllocateGraphicsBuffer(
	VSR_Renderer* renderer);

VkCommandBuffer
Renderer_CommandPoolAllocateTransferBuffer(
	VSR_Renderer* renderer);

void
Renderer_CommandPoolSubmitTransferBuffer(
	VSR_Renderer* renderer,
	VkCommandBuffer buff,
	VkFence fence);

int
Renderer_CommandBufferRecordStart(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VkCommandBuffer cBuff);


int
Renderer_CommandBufferRecordEnd(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VkCommandBuffer cBuff);

#endif //VSR_COMMAND_POOL_H

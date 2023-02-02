#ifndef VSR_COMMAND_POOL_H
#define VSR_COMMAND_POOL_H

#include "vulkan/vulkan.h"


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


typedef struct GraphicsPipeline_CommandPoolCreateInfo GraphicsPipeline_CommandPoolCreateInfo;
struct GraphicsPipeline_CommandPoolCreateInfo
{
	VkCommandPoolCreateInfo commandPoolCreateInfo;
};

typedef struct GraphicsPipeline_CommandPool GraphicsPipeline_CommandPool;
struct GraphicsPipeline_CommandPool
{
	VkCommandPool graphicsPool;
	VkCommandPool transferPool;
};


SDL_bool
GraphicsPipeline_CommandPoolPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo);


SDL_bool
GraphicsPipeline_CommandPoolCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_GraphicsPipelineCreateInfo* createInfo);

void
GraphicsPipeline_CommandPoolDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline
);

VkCommandBuffer
GraphicsPipeline_CommandPoolAllocateGraphicsBuffer(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline);

VkCommandBuffer
GraphicsPipeline_CommandPoolAllocateTransferBuffer(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline);

int
GraphicsPipeline_CommandBufferRecordStart(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VkCommandBuffer cBuff
	);


int
GraphicsPipeline_CommandBufferRecordEnd(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VkCommandBuffer cBuff);

#endif //VSR_COMMAND_POOL_H

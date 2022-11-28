#ifndef VSR_COMMANDPOOL_H
#define VSR_COMMANDPOOL_H

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
	VkCommandBuffer* commandBuffers;
};


SDL_bool
GraphicsPipeline_CommandPoolPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo,
	GraphicsPipeline_CreateInfoSubStructs * subStructs);


SDL_bool
GraphicsPipeline_CommandPoolCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	GraphicsPipeline_CreateInfoSubStructs* subStructs);

void
GraphicsPipeline_CommandPoolDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline
);

#endif //VSR_COMMANDPOOL_H

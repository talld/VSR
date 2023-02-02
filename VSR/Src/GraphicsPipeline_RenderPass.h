#ifndef VSR_RENDERPASS_H
#define VSR_RENDERPASS_H

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


typedef struct GraphicsPipeline_RenderPassCreateInfo GraphicsPipeline_RenderPassCreateInfo;
struct GraphicsPipeline_RenderPassCreateInfo
{
	VkRenderPassCreateInfo createInfo;
};

typedef struct GraphicsPipeline_RenderPass GraphicsPipeline_RenderPass;
struct GraphicsPipeline_RenderPass
{
	VkRenderPass renderPass;
};

SDL_bool
GraphicsPipeline_RenderPassPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo);


SDL_bool
GraphicsPipeline_RenderPassCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_GraphicsPipelineCreateInfo* createInfo);

void
GraphicsPipeline_RenderPassDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline
	);

#endif //VSR_RENDERPASS_H

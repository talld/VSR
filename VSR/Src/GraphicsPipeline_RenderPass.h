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

typedef struct GrapicsPipeline_RenderPass GrapicsPipeline_RenderPass;
struct GrapicsPipeline_RenderPass
{
	VkRenderPass renderPass;
};

SDL_bool
GraphicsPipeline_RenderPassPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo,
	GraphicsPipeline_CreateInfoSubStructs* subStructs);


SDL_bool
GraphicsPipeline_RenderPassCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	GraphicsPipeline_CreateInfoSubStructs* subStructs);

void
GraphicsPipeline_RenderPassDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline
	);

#endif //VSR_RENDERPASS_H
#ifndef VSR_FRAMEBUFFER_H
#define VSR_FRAMEBUFFER_H

#include <vulkan/vulkan.h>


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


typedef struct GraphicsPipeline_FramebufferCreateInfo GraphicsPipeline_FramebufferCreateInfo;
struct GraphicsPipeline_FramebufferCreateInfo
{
	VkFramebufferCreateInfo framebufferCreateInfo;
};

typedef struct GraphicsPipeline_Framebuffer GraphicsPipeline_Framebuffer;
struct GraphicsPipeline_Framebuffer
{
	// has the same number
	VkFramebuffer* framebuffers;
};

SDL_bool
GraphicsPipeline_FramebufferPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* graphicsPipelineCreateInfo,
	GraphicsPipeline_CreateInfoSubStructs* subStructs);


SDL_bool
GraphicsPipeline_FramebufferCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	GraphicsPipeline_CreateInfoSubStructs* subStructs);

void
GraphicsPipeline_FramebufferDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline
);

#endif //VSR_FRAMEBUFFER_H
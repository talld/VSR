#ifndef VSR_SUITE_RENDERER_RENDERPASS_H
#define VSR_SUITE_RENDERER_RENDERPASS_H

#include "vulkan/vulkan.h"

typedef struct VSR_RenderPassCreateInfo VSR_RenderPassCreateInfo;
struct VSR_RenderPassCreateInfo
{
	VkRenderPassCreateInfo createInfo;
};

typedef struct VSR_RenderPass VSR_RenderPass;
struct VSR_RenderPass
{
	VkRenderPass renderPass;
};

SDL_bool
VSR_RendererPopulateRenderPassCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs);


SDL_bool
VSR_CreateRenderPass(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs);

#endif //VSR_SUITE_RENDERER_RENDERPASS_H

#ifndef VSR_RENDERPASS_H
#define VSR_RENDERPASS_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

////////////////////////////////
/// Renderer pre-declaration ///
////////////////////////////////
typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs;

typedef struct Renderer_RenderPass Renderer_RenderPass;
struct Renderer_RenderPass
{
	VkRenderPass renderPass;
};

SDL_bool
Renderer_RenderPassCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo
);

void
Renderer_RenderPassDestroy(
	VSR_Renderer* renderer
);

#endif //VSR_RENDERPASS_H

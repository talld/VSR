#ifndef VSR_RENDERPASS_H
#define VSR_RENDERPASS_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

#include "VSR_Image.h"

typedef struct VSR_RenderPass VSR_RenderPass;
struct VSR_RenderPass
{
	VkRenderPass renderPass;
};

VSR_RenderPass*
VSR_RenderPassCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_ImageView* colourView,
	VSR_ImageView* depthView
);

void
VSR_RenderPassDestroy(
	VSR_Renderer* renderer,
	VSR_RenderPass* renderPass
);

#endif //VSR_RENDERPASS_H

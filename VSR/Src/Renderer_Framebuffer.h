#ifndef VSR_FRAMEBUFFER_H
#define VSR_FRAMEBUFFER_H

#include <vulkan/vulkan.h>

typedef struct VSR_FramebufferCreateInfo VSR_FramebufferCreateInfo;
struct VSR_FramebufferCreateInfo
{
	VkFramebufferCreateInfo framebufferCreateInfo;
};

typedef struct VSR_Framebuffer VSR_Framebuffer;
struct VSR_Framebuffer
{
	// has the same number
	VkFramebuffer* framebuffers;
};

SDL_bool
VSR_FramebufferPopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs);


SDL_bool
VSR_FramebufferCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs);

void
VSR_FramebufferDestroy(
	VSR_Renderer* renderer
);

#endif //VSR_FRAMEBUFFER_H

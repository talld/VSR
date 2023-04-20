#ifndef VSR_FRAMEBUFFER_H
#define VSR_FRAMEBUFFER_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

#include "VSR_Image.h"

////////////////////////////////
/// Renderer pre-declaration ///
////////////////////////////////
typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs;

typedef struct VSR_Framebuffer VSR_Framebuffer;
struct VSR_Framebuffer
{
	VkFramebuffer frame;
};

VSR_Framebuffer*
VSR_CreateFramebuffer(
	VSR_Renderer* renderer,
	VSR_ImageView* imageView
);

void
VSR_DestroyFramebuffer(
	VSR_Renderer* renderer,
	VSR_Framebuffer* framebuffer
);

#endif //VSR_FRAMEBUFFER_H

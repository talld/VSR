#ifndef VSR_IMAGE_H
#define VSR_IMAGE_H

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

#include "Renderer_Memory.h"

typedef struct VSR_Image VSR_Image;
struct VSR_Image
{
	SDL_Surface src;
	VkImage image;
	VkFormat format;
	VkDeviceSize imageBufferSize;
	Renderer_MemoryAlloc* alloc;
};

typedef struct VSR_ImageView VSR_ImageView;
struct VSR_ImageView
{
	VkImage image;
	VkFormat format;
	VkImageView imageView;
};

VSR_Image*
VSR_ImageCreate(
	VSR_Renderer* renderer,
	SDL_Surface* surface,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags useFlags);

VSR_ImageView
VSR_ImageViewCreate(
	VSR_Renderer* renderer,
	VkImage image,
	VkFormat imageFormat,
	VkImageAspectFlags aspectFlags);

void
VSR_ImageViewDestroy(
	VSR_Renderer* renderer,
	VSR_ImageView* imageView);

#endif //VSR_IMAGE_H

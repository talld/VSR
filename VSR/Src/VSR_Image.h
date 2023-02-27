#ifndef VSR_IMAGE_H
#define VSR_IMAGE_H

#include <vulkan/vulkan.h>

typedef struct VSR_Image VSR_Image;
struct VSR_Image
{
	SDL_Surface src;
	VkImage image;
};

typedef struct VSR_ImageView VSR_ImageView;
struct VSR_ImageView
{
	VkImage image;
	VkFormat format;
	VkImageView imageView;
};

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

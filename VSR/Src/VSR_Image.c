#include "VSR_Image.h"

#include "VSR_Renderer.h"
#include "VSR_error.h"

VkImage createImage(VSR_Renderer* renderer,
	   uint32_t w,
	   uint32_t h,
	   VkFormat format,
	   VkImageTiling tiling,
	   VkImageUsageFlags usage)
{
	VkImageCreateInfo imageCreateInfo;

	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.flags = 0L;

	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = w;
	imageCreateInfo.extent.height = h;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = format;
	imageCreateInfo.tiling = tiling;
	imageCreateInfo.usage = usage;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkImage image;

	vkCreateImage(
		renderer->subStructs->logicalDevice.device,
		&imageCreateInfo,
		VSR_GetAllocator(),
		&image
	);

	return image;
}


//==============================================================================
// VSR_ImageCreate
//------------------------------------------------------------------------------
VSR_Image*
VSR_ImageCreate(
	VSR_Renderer* renderer,
	SDL_Surface* surface,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags useFlags)
{
	VSR_Image* image = SDL_malloc(sizeof(VSR_Image));

	size_t surfaceExtent = (surface->w * surface->h);

	// it's possible we've just passed the surface only fo wh
	if(surface->pixels)
	{
		surface = SDL_ConvertSurfaceFormat(surface,SDL_PIXELFORMAT_RGBA8888, 0);
		image->imageBufferSize = surface->format->BytesPerPixel * surfaceExtent;
	}
	else
	{

		image->imageBufferSize = 4 * surfaceExtent;
	}

	image->image = createImage(
		renderer,
		surface->w,
		surface->h,
		format,
		tiling,
		useFlags
		);

	image->format = format;

	image->alloc = 	Renderer_MemoryAllocate(
		renderer,
		&renderer->subStructs->USDGPUBuffer,
		image->imageBufferSize
		);

	vkBindImageMemory(
		renderer->subStructs->logicalDevice.device,
		image->image,
		renderer->subStructs->USDGPUBuffer.memory,
		image->alloc->offset
	);

	return image;
}





//==============================================================================
// VSR_ImageViewCreate
//------------------------------------------------------------------------------
VSR_ImageView
VSR_ImageViewCreate(
	VSR_Renderer* renderer,
	VkImage image,
	VkFormat imageFormat,
	VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewCreateInfo;

	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.flags = 0;
	viewCreateInfo.pNext = NULL;
	viewCreateInfo.image = image;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = imageFormat;
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	viewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;

	VkResult err;
	VkImageView view;

	err = vkCreateImageView(
		renderer->subStructs->logicalDevice.device,
		&viewCreateInfo,
		VSR_GetAllocator(),
		&view
		);

	VSR_ImageView imageView;
	if(err == VK_SUCCESS)
	{
		imageView.image     = image;
		imageView.format    = imageFormat;
		imageView.imageView = view;
	}

	return imageView;
}





//==============================================================================
// VSR_ImageViewDestroy
//------------------------------------------------------------------------------
void
VSR_ImageViewDestroy(
	VSR_Renderer* renderer,
	VSR_ImageView* imageView)
{
	vkDestroyImageView(renderer->subStructs->logicalDevice.device,
					   imageView->imageView, VSR_GetAllocator());

	VSR_ImageView blank = {.image = VK_NULL_HANDLE,
						   .format = VK_FORMAT_UNDEFINED,
						   .imageView = VK_NULL_HANDLE,
						   };
	*imageView = blank;
}
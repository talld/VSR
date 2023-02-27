#include "VSR_Image.h"

#include "VSR_Renderer.h"
#include "VSR_error.h"





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
#include "Renderer_Swapchain.h"

#include <SDL_vulkan.h>

#include "VSR_Renderer.h"
#include "VSR_error.h"

//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_SwapchainPopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo)
{

	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}





//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_SwapchainCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo)
{
	///////////////////////////////////
	/// Fill missing swapchain data ///
	///////////////////////////////////
	createInfo->swapchainCreateInfo.createInfo.sType =
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

	createInfo->swapchainCreateInfo.createInfo.pNext = NULL;
	createInfo->swapchainCreateInfo.createInfo.flags = 0;

	createInfo->swapchainCreateInfo.createInfo.surface =
		renderer->surface.surface;

	createInfo->swapchainCreateInfo.createInfo.imageFormat =
		renderer->surface.surfaceFormat;

	createInfo->swapchainCreateInfo.createInfo.imageColorSpace =
		renderer->surface.surfaceColourSpace;

	createInfo->swapchainCreateInfo.createInfo.presentMode =
		renderer->surface.surfacePresentMode;

	createInfo->swapchainCreateInfo.createInfo.imageExtent.width =
		renderer->surface.surfaceWidth;

	createInfo->swapchainCreateInfo.createInfo.imageExtent.height =
		renderer->surface.surfaceHeight;

	// plain old double buffering
	createInfo->swapchainCreateInfo.createInfo.minImageCount =
		renderer->surface.surfaceCapabilities.minImageCount;

	createInfo->swapchainCreateInfo.createInfo.imageArrayLayers = 1;

	createInfo->swapchainCreateInfo.createInfo.imageUsage =
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	createInfo->swapchainCreateInfo.createInfo.preTransform =
		renderer->surface.surfaceCapabilities.currentTransform;

	createInfo->swapchainCreateInfo.createInfo.compositeAlpha =
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo->swapchainCreateInfo.createInfo.clipped = VK_TRUE;

	createInfo->swapchainCreateInfo.createInfo.oldSwapchain = VK_NULL_HANDLE;

	/////////////////////////////////////////////////////////////////
	/// Set the weather the swapchain may have to be moved around ///
	/////////////////////////////////////////////////////////////////
	if(renderer->deviceQueues.QCanPresent[kGraphicsQueueIndex])
	{ // Fast
		createInfo->swapchainCreateInfo.createInfo.imageSharingMode =
			VK_SHARING_MODE_EXCLUSIVE;

		createInfo->swapchainCreateInfo.createInfo.queueFamilyIndexCount = 0;
		createInfo->swapchainCreateInfo.createInfo.pQueueFamilyIndices = NULL;
	}
	else
	{ // slow
		uint32_t graphicsAndPresentQueues[2] = {
			// compute is the only queue that can support present
			// ( I'm choosing to ignore transport present it's too awkward...)
			renderer->deviceQueues.QFamilyIndexes[kGraphicsQueueIndex],
			renderer->deviceQueues.QFamilyIndexes[kComputeQueueIndex],
		};

		createInfo->swapchainCreateInfo.createInfo.imageSharingMode =
			VK_SHARING_MODE_CONCURRENT;

		createInfo->swapchainCreateInfo.createInfo.queueFamilyIndexCount = 2;
		createInfo->swapchainCreateInfo.createInfo.pQueueFamilyIndices =
			graphicsAndPresentQueues;
	}

	////////////////////////////
	/// Create the Swapchain ///
	////////////////////////////
	VkResult err;
	VkSwapchainKHR swapchain;

	err = vkCreateSwapchainKHR(renderer->logicalDevice.device,
						 &createInfo->swapchainCreateInfo.createInfo,
						 VSR_GetAllocator(),
						 &swapchain);

	if(err != VK_SUCCESS)
	{
        VSR_Error("Failed to create swapchain: %s",
				VSR_VkErrorToString(err));
		goto FAIL;
	}

	renderer->swapchain.swapchain = swapchain;

	////////////////////////
	/// Swapchain images ///
	////////////////////////
	uint32_t swapchainImageCount;
	vkGetSwapchainImagesKHR(
		renderer->logicalDevice.device,
		renderer->swapchain.swapchain,
		&swapchainImageCount,
		NULL);

	size_t imageListSize = swapchainImageCount * sizeof(VkImage);
	VkImage* imageList = SDL_malloc(imageListSize);

	vkGetSwapchainImagesKHR(
		renderer->logicalDevice.device,
		renderer->swapchain.swapchain,
		&swapchainImageCount,
		imageList);

	renderer->swapchain.imageViewCount = swapchainImageCount;

	size_t imageViewsListSize = swapchainImageCount * sizeof(VSR_ImageView);
	renderer->swapchain.pImageViews = SDL_malloc(imageViewsListSize);

	for(size_t i = 0; i < swapchainImageCount; i++)
	{
		VkImage image = imageList[i];

		VSR_ImageView* imageView =
		VSR_ImageViewCreate(
			renderer,
			image,
			renderer->surface.surfaceFormat,
			VK_IMAGE_ASPECT_COLOR_BIT);

		renderer->swapchain.pImageViews[i] = imageView;
	}

	SDL_free((void*) imageList);

	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}





//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
void
VSR_SwapchainDestroy(
	VSR_Renderer* renderer
)
{
	size_t count = renderer->swapchain.imageViewCount;

	for(size_t i = 0; i < count; i++)
	{
		VSR_ImageView* pView =
			renderer->swapchain.pImageViews[i];

			VSR_ImageViewDestroy(renderer, pView);
	}

	vkDestroySwapchainKHR(
		renderer->logicalDevice.device,
		renderer->swapchain.swapchain,
		VSR_GetAllocator()
		);

	SDL_free(renderer->swapchain.pImageViews);
}
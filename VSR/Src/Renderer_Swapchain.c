#include "Renderer_Swapchain.h"

#include <SDL_vulkan.h>

#include "Renderer.h"
#include "VSR_error.h"

//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_SwapchainPopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs)
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
	VSR_RendererCreateInfoSubStructs* subStructs)
{
	///////////////////////////////////
	/// Fill missing swapchain data ///
	///////////////////////////////////
	subStructs->swapchainCreateInfo.createInfo.sType =
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

	subStructs->swapchainCreateInfo.createInfo.pNext = NULL;
	subStructs->swapchainCreateInfo.createInfo.flags = 0;

	subStructs->swapchainCreateInfo.createInfo.surface =
		renderer->subStructs->surface.surface;

	subStructs->swapchainCreateInfo.createInfo.imageFormat =
		renderer->subStructs->surface.surfaceFormat;

	subStructs->swapchainCreateInfo.createInfo.imageColorSpace =
		renderer->subStructs->surface.surfaceColourSpace;

	subStructs->swapchainCreateInfo.createInfo.presentMode =
		renderer->subStructs->surface.surfacePresentMode;

	subStructs->swapchainCreateInfo.createInfo.imageExtent.width =
		renderer->subStructs->surface.surfaceWidth;

	subStructs->swapchainCreateInfo.createInfo.imageExtent.height =
		renderer->subStructs->surface.surfaceHeight;

	// plain old double buffering
	subStructs->swapchainCreateInfo.createInfo.minImageCount =
		renderer->subStructs->surface.surfaceCapabilities.minImageCount;

	subStructs->swapchainCreateInfo.createInfo.imageArrayLayers = 1;

	subStructs->swapchainCreateInfo.createInfo.imageUsage =
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	subStructs->swapchainCreateInfo.createInfo.preTransform =
		renderer->subStructs->surface.surfaceCapabilities.currentTransform;

	subStructs->swapchainCreateInfo.createInfo.compositeAlpha =
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	subStructs->swapchainCreateInfo.createInfo.clipped = VK_TRUE;

	subStructs->swapchainCreateInfo.createInfo.oldSwapchain = VK_NULL_HANDLE;

	/////////////////////////////////////////////////////////////////
	/// Set the weather the swapchain may have to be moved around ///
	/////////////////////////////////////////////////////////////////
	if(renderer->subStructs->deviceQueues.graphicsCanPresent)
	{ // Fast
		subStructs->swapchainCreateInfo.createInfo.imageSharingMode =
			VK_SHARING_MODE_EXCLUSIVE;

		subStructs->swapchainCreateInfo.createInfo.queueFamilyIndexCount = 0;
		subStructs->swapchainCreateInfo.createInfo.pQueueFamilyIndices = NULL;
	}
	else
	{ // slow
		uint32_t graphicsAndPresentQueues[2] = {
			// compute is the only queue that can support present
			// ( I'm choosing to ignore transport present it's too awkward...)
			subStructs->deviceQueuesCreateInfo.graphicsQueueCreateInfoIndex,
			subStructs->deviceQueuesCreateInfo.computeQueueCreateInfoIndex,
		};

		subStructs->swapchainCreateInfo.createInfo.imageSharingMode =
			VK_SHARING_MODE_CONCURRENT;

		subStructs->swapchainCreateInfo.createInfo.queueFamilyIndexCount = 2;
		subStructs->swapchainCreateInfo.createInfo.pQueueFamilyIndices =
			graphicsAndPresentQueues;
	}

	////////////////////////////
	/// Create the Swapchain ///
	////////////////////////////
	VkResult err;
	VkSwapchainKHR swapchain;

	err = vkCreateSwapchainKHR(renderer->subStructs->logicalDevice.device,
						 &subStructs->swapchainCreateInfo.createInfo,
						 VSR_GetAllocator(),
						 &swapchain);

	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create swapchain: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
		goto FAIL;
	}

	renderer->subStructs->swapchain.swapchain = swapchain;

	////////////////////////
	/// Swapchain images ///
	////////////////////////
	uint32_t swapchainImageCount;
	vkGetSwapchainImagesKHR(
		renderer->subStructs->logicalDevice.device,
		renderer->subStructs->swapchain.swapchain,
		&swapchainImageCount,
		NULL);

	size_t imageListSize = swapchainImageCount * sizeof(VkImage);
	VkImage* imageList = SDL_malloc(imageListSize);

	vkGetSwapchainImagesKHR(
		renderer->subStructs->logicalDevice.device,
		renderer->subStructs->swapchain.swapchain,
		&swapchainImageCount,
		imageList);

	renderer->subStructs->swapchain.imageViewCount = swapchainImageCount;

	size_t imageViewsListSize = swapchainImageCount * sizeof(VSR_ImageView);
	renderer->subStructs->swapchain.imageViews = SDL_malloc(imageViewsListSize);

	for(size_t i = 0; i < swapchainImageCount; i++)
	{
		VkImage image = imageList[i];

		VSR_ImageView imageView =
		VSR_ImageViewCreate(
			renderer,
			image,
			renderer->subStructs->surface.surfaceFormat,
			VK_IMAGE_ASPECT_COLOR_BIT);

		renderer->subStructs->swapchain.imageViews[i] = imageView;
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
	size_t count = renderer->subStructs->swapchain.imageViewCount;

	for(size_t i = 0; i < count; i++)
	{
		VSR_ImageView* pView =
			&renderer->subStructs->swapchain.imageViews[i];

			VSR_ImageViewDestroy(renderer, pView);
	}

	SDL_free(renderer->subStructs->swapchain.imageViews);
}
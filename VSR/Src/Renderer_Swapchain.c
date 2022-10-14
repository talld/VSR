#include "Renderer_Swapchain.h"

#include "Renderer.h"


//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_SwapchainPopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs)
{

	VkSwapchainCreateInfoKHR* swapchainCreateInfo  =
								&subStructs->swapchainCreateInfo.createInfo;


	swapchainCreateInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo->pNext = NULL;
	swapchainCreateInfo->flags = 0;
	swapchainCreateInfo->minImageCount = 2; // double buffing



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
	subStructs->swapchainCreateInfo.createInfo.surface =
		renderer->subStructs->surface.surface;

	subStructs->swapchainCreateInfo.createInfo.imageFormat =
		renderer->subStructs->surface.surfaceFormat;

	subStructs->swapchainCreateInfo.createInfo.presentMode =
		renderer->subStructs->surface.surfacePresentMode;

	VkSwapchainKHR swapchain;

	vkCreateSwapchainKHR(renderer->subStructs->logicalDevice.device,
						 &subStructs->swapchainCreateInfo.createInfo,
						 VSR_GetAllocator(),
						 &swapchain);

	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}
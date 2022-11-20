#ifndef VSR_SUITE_RENDERER_SWAPCHAIN_H
#define VSR_SUITE_RENDERER_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include "Renderer_ImageView.h"

typedef struct VSR_SwapchainCreateInfo VSR_SwapchainCreateInfo;
struct VSR_SwapchainCreateInfo
{
	VkSwapchainCreateInfoKHR   createInfo;
};


typedef struct VSR_Swapchain VSR_Swapchain;
struct VSR_Swapchain
{
	VkSwapchainKHR   swapchain;
	size_t           imageViewCount;
	VSR_ImageView*   imageViews;
};


SDL_bool
VSR_SwapchainPopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs);


SDL_bool
VSR_SwapchainCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs);

void
VSR_SwapchainDestroy(
	VSR_Renderer* renderer
	);

#endif // VSR_SUITE_RENDERER_SWAPCHAIN_H

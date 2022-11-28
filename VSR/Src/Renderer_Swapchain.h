#ifndef VSR_SWAPCHAIN_H
#define VSR_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include "Renderer_ImageView.h"


////////////////////////////////
/// Renderer pre-declaration ///
////////////////////////////////
typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs;


typedef struct Renderer_SwapchainCreateInfo Renderer_SwapchainCreateInfo;
struct Renderer_SwapchainCreateInfo
{
	VkSwapchainCreateInfoKHR   createInfo;
};


typedef struct Renderer_Swapchain Renderer_Swapchain;
struct Renderer_Swapchain
{
	VkSwapchainKHR   swapchain;
	size_t           imageViewCount;
	VSR_ImageView*   imageViews;
};


SDL_bool
VSR_SwapchainPopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	Renderer_CreateInfoSubStructs* subStructs);


SDL_bool
VSR_SwapchainCreate(
	VSR_Renderer* renderer,
	Renderer_CreateInfoSubStructs* subStructs);

void
VSR_SwapchainDestroy(
	VSR_Renderer* renderer
	);

#endif // VSR_SWAPCHAIN_H

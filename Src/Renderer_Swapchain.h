#ifndef VSR_SWAPCHAIN_H
#define VSR_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include "VSR_Image.h"


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
	VkImage*         pSwapchainImages;
	VSR_ImageView**  pImageViews;
};


SDL_bool
VSR_SwapchainPopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo);


SDL_bool
VSR_SwapchainCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo);

void
VSR_SwapchainDestroy(
	VSR_Renderer* renderer
	);

#endif // VSR_SWAPCHAIN_H

#ifndef VSR_SUITE_RENDERER_SWAPCHAIN_H
#define VSR_SUITE_RENDERER_SWAPCHAIN_H

#include <vulkan/vulkan.h>


typedef struct VSR_SwapchainCreateInfo VSR_SwapchainCreateInfo;
struct VSR_SwapchainCreateInfo
{
	VkSwapchainCreateInfoKHR   createInfo;
};


typedef struct VSR_Swapchain VSR_Swapchain;
struct VSR_Swapchain
{
	VkSwapchainKHR   swapchain;
};


SDL_bool
VSR_SwapchainPopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs);


SDL_bool
VSR_SwapchainCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs);


#endif // VSR_SUITE_RENDERER_SWAPCHAIN_H

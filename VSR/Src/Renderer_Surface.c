#include "Renderer_Surface.h"

#include <SDL_vulkan.h>

#include "VSR_Renderer.h"
#include "VSR_error.h"

//==============================================================================
// VSR_CreateInstance
//------------------------------------------------------------------------------
SDL_bool
VSR_SurfaceCreate(
	VSR_Renderer* renderer,
	Renderer_CreateInfoSubStructs* subStructs)
{
	SDL_bool err;

	//////////////////////////
	/// Create the surface ///
	//////////////////////////
	err = SDL_Vulkan_CreateSurface(renderer->SDLWindow,
							 renderer->subStructs->instance.instance,
							 &renderer->subStructs->surface.surface);

	if(err != SDL_TRUE)
	{
        VSR_Error("Failed to create window surface: %s",
				SDL_GetError());
		goto FAIL;
	}

	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}

Renderer_Surface
VSR_SurfaceGetSurfaceFormatFromDevice(
	VSR_Renderer* renderer,
	VkPhysicalDevice device)
{
	Renderer_Surface surface;
	surface.surface = renderer->subStructs->surface.surface;

	///////////////////////////
	/// surfaceCapabilities ///
	///////////////////////////
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,
											  surface.surface,
											  &surface.surfaceCapabilities);

	//////////////
	/// Format ///
	//////////////
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		device,
		renderer->subStructs->surface.surface,
		&formatCount,
		NULL);

	size_t formatListSize = formatCount * sizeof(VkSurfaceFormatKHR);
	VkSurfaceFormatKHR* formatList = SDL_malloc(formatListSize);

	vkGetPhysicalDeviceSurfaceFormatsKHR(
		device,
		renderer->subStructs->surface.surface,
		&formatCount,
		formatList);

	for(size_t i = 0; i < formatCount; i++)
	{
		VkFormat format = formatList[i].format;
		VkColorSpaceKHR colorSpace = formatList[i].colorSpace;

		if(format == VK_FORMAT_B8G8R8A8_SRGB
		&& colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			surface.surfaceFormat = format;
			surface.surfaceColourSpace = colorSpace;
		}
	}

	SDL_free((void*) formatList);

	////////////////////
	/// Present mode ///
	////////////////////
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		device,
		renderer->subStructs->surface.surface,
		&presentModeCount,
		NULL
		);

	size_t presentModeListSize = presentModeCount * sizeof(VkPresentModeKHR);
	VkPresentModeKHR* presentModeList = SDL_malloc(presentModeListSize);

	vkGetPhysicalDeviceSurfacePresentModesKHR(
		device,
		renderer->subStructs->surface.surface,
		&presentModeCount,
		presentModeList
	);

	surface.surfacePresentMode = VK_PRESENT_MODE_FIFO_KHR; // default

	// try to get mailbox if it's there as it has better frame timings
	for(size_t i = 0; i < presentModeCount; i++)
	{
		if(presentModeList[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			surface.surfacePresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		}
	}

	SDL_free((void*) presentModeList);

	//////////////////
	/// Resolution ///
	//////////////////
	int w;
	int h;
	SDL_Vulkan_GetDrawableSize(renderer->SDLWindow, &w, &h);

	surface.surfaceWidth  = w;
	surface.surfaceHeight = h;

	return surface;
}





//==============================================================================
// VSR_SurfaceDestroy
//------------------------------------------------------------------------------
void
VSR_SurfaceDestroy(
	VSR_Renderer* renderer)
{
	vkDestroySurfaceKHR(renderer->subStructs->instance.instance,
	                    renderer->subStructs->surface.surface,
	                    VSR_GetAllocator());
}
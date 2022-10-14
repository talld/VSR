#ifndef VSR_SUITE_RENDERER_SURFACE_H
#define VSR_SUITE_RENDERER_SURFACE_H

#include <vulkan/vulkan.h>


typedef struct VSR_SurfaceCreateInfo VSR_SurfaceCreateInfo;
struct VSR_SurfaceCreateInfo
{

};


typedef struct VSR_Surface VSR_Surface;
struct VSR_Surface
{
	VkSurfaceKHR               surface;
	VkSurfaceCapabilitiesKHR   surfaceCapabilities2;
	VkFormat                   surfaceFormat;
	VkColorSpaceKHR            surfaceColourSpace;
	VkPresentModeKHR           surfacePresentMode;
};

SDL_bool
VSR_SurfaceCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs);

VSR_Surface
VSR_SurfaceGetSurfaceFormatFromDevice(
	VSR_Renderer* renderer,
	VkPhysicalDevice device);

#endif //VSR_SUITE_RENDERER_SURFACE_H

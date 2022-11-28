#ifndef VSR_SURFACE_H
#define VSR_SURFACE_H

#include <vulkan/vulkan.h>


////////////////////////////////
/// Renderer pre-declaration ///
////////////////////////////////
typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs;


typedef struct Renderer_SurfaceCreateInfo Renderer_SurfaceCreateInfo;
struct Renderer_SurfaceCreateInfo
{

};


typedef struct Renderer_Surface Renderer_Surface;
struct Renderer_Surface
{
	VkSurfaceKHR               surface;
	VkSurfaceCapabilitiesKHR   surfaceCapabilities;
	VkFormat                   surfaceFormat;
	VkColorSpaceKHR            surfaceColourSpace;
	VkPresentModeKHR           surfacePresentMode;
	int                        surfaceWidth;
	int                        surfaceHeight;
};

SDL_bool
VSR_SurfaceCreate(
	VSR_Renderer* renderer,
	Renderer_CreateInfoSubStructs* subStructs);

Renderer_Surface
VSR_SurfaceGetSurfaceFormatFromDevice(
	VSR_Renderer* renderer,
	VkPhysicalDevice device);

void
VSR_SurfaceDestroy(
	VSR_Renderer* renderer);

#endif //VSR_SURFACE_H

#include "Renderer_Surface.h"


struct RendererSurface
{
	VkSurfaceKHR             surface;
	VkSurfaceCapabilitiesKHR surfaceCapabilities2;
	VkFormat                 surfaceFormat;
	VkColorSpaceKHR          surfaceColourSpace;
};

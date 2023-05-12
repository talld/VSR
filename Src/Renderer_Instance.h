#ifndef VSR_INSTANCE_H
#define VSR_INSTANCE_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>


typedef struct Renderer_InstanceCreateInfo Renderer_InstanceCreateInfo;
struct Renderer_InstanceCreateInfo
{
	VkApplicationInfo      applicationInfo;
	VkInstanceCreateInfo   createInfo;
};

typedef struct Renderer_Instance Renderer_Instance;
struct Renderer_Instance
{
	VkInstance   instance;
};

SDL_bool
VSR_InstancePopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo);

SDL_bool
VSR_InstanceCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo);

void
VSR_InstanceDestroy
	(VSR_Renderer* renderer);

#endif // VSR_INSTANCE_H

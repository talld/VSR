#ifndef VSR_INSTANCE_H
#define VSR_INSTANCE_H

#include <vulkan/vulkan.h>

typedef struct VSR_InstanceCreateInfo VSR_InstanceCreateInfo;
struct VSR_InstanceCreateInfo
{
	VkApplicationInfo      applicationInfo;
	VkInstanceCreateInfo   createInfo;
};

typedef struct VSR_Instance VSR_Instance;
struct VSR_Instance
{
	VkInstance   instance;
};

SDL_bool
VSR_InstancePopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs);

SDL_bool
VSR_InstanceCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs);

void
VSR_InstanceDestroy
	(VSR_Renderer* renderer);

#endif // VSR_INSTANCE_H

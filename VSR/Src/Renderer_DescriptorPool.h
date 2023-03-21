#ifndef VSR_DESCRIPTOR_POOL_H
#define VSR_DESCRIPTOR_POOL_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

////////////////////////////////
/// Renderer pre-declaration ///
////////////////////////////////
typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs;

////////////////////////////////////////
/// GraphicsPipeline pre-declaration ///
////////////////////////////////////////
typedef struct GraphicsPipeline_CreateInfoSubStructs GraphicsPipeline_CreateInfoSubStructs;
struct GraphicsPipeline_CreateInfoSubStructs;


typedef struct Renderer_DescriptorPoolCreateInfo Renderer_DescriptorPoolCreateInfo;
struct Renderer_DescriptorPoolCreateInfo
{

	VkDescriptorSetLayoutBinding  textureBinding;
	VkDescriptorSetLayoutCreateInfo globalLayout;


	VkDescriptorSetLayoutBinding  userBindings[4];
	VkDescriptorSetLayoutCreateInfo userLayout;
};

typedef struct Renderer_DescriptorPool Renderer_DescriptorPool;
struct Renderer_DescriptorPool
{
	VkDescriptorPool  globalPool;

	VkDescriptorSetLayout  globalLayout;
	VkDescriptorSet globalSet;

	VkDescriptorSetLayout userLayout;
	VkDescriptorSet userSet;

};


SDL_bool
Renderer_DescriptorPoolPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo);


SDL_bool
Renderer_DescriptorPoolCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo);

void
Renderer_DescriptorPoolDestroy(
	VSR_Renderer* renderer);


#endif //VSR_DESCRIPTOR_POOL_H

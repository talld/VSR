#ifndef VSR_DESCRIPTOR_POOL_H
#define VSR_DESCRIPTOR_POOL_H

#include "vulkan/vulkan.h"


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


typedef struct GraphicsPipeline_DescriptorPoolCreateInfo GraphicsPipeline_DescriptorPoolCreateInfo;
struct GraphicsPipeline_DescriptorPoolCreateInfo
{

	VkDescriptorSetLayoutBinding  textureBinding;
	VkDescriptorSetLayoutCreateInfo globalLayout;
};

typedef struct GraphicsPipeline_DescriptorPool GraphicsPipeline_DescriptorPool;
struct GraphicsPipeline_DescriptorPool
{
	VkDescriptorSetLayout  globalLayout;
	VkDescriptorPool  globalPool;
	VkDescriptorSet globalSet;
};


SDL_bool
GraphicsPipeline_DescriptorPoolPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo);


SDL_bool
GraphicsPipeline_DescriptorPoolCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_GraphicsPipelineCreateInfo* createInfo);

void
GraphicsPipeline_DescriptorPoolDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline
);


#endif //VSR_DESCRIPTOR_POOL_H

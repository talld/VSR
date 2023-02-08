#include "GraphicsPipeline_DescriptorPool.h"
#include "VSR_GraphicsPipeline.h"

SDL_bool
GraphicsPipeline_DescriptorPoolPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo)
{


	VkDescriptorSetLayoutCreateInfo* perModelCreateInfo =
		&createInfo->subStructs->descriptorPoolCreateInfo.perModelLayout;

	VkDescriptorSetLayoutCreateInfo* perSceneCreateInfo =
		&createInfo->subStructs->descriptorPoolCreateInfo.perModelLayout;


	perModelCreateInfo->sType =
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;


	perSceneCreateInfo->sType =
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

	return SDL_TRUE;
}


SDL_bool
GraphicsPipeline_DescriptorPoolCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_GraphicsPipelineCreateInfo* createInfo)
{
	return SDL_FALSE;
}

void
GraphicsPipeline_DescriptorPoolDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline)
{

}
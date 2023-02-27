#include "GraphicsPipeline_DescriptorPool.h"

#include "VSR_Renderer.h"
#include "VSR_GraphicsPipeline.h"
#include "VSR_error.h"

SDL_bool
GraphicsPipeline_DescriptorPoolPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo)
{
	///////////////
	/// aliases ///
	///////////////
	VkDescriptorSetLayoutBinding* textureBinding =
		&createInfo->subStructs->descriptorPoolCreateInfo.textureBinding;

	VkDescriptorSetLayoutCreateInfo* globalLayoutCreateInfo =
		&createInfo->subStructs->descriptorPoolCreateInfo.globalLayout;

	/////////////////////////////
	/// global descriptors ///
	/////////////////////////////

	textureBinding->binding = 0;
	textureBinding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureBinding->descriptorCount = createInfo->texturePoolSize;

	globalLayoutCreateInfo->sType =
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

	globalLayoutCreateInfo->flags = 0L;
	globalLayoutCreateInfo->pNext = NULL;

	globalLayoutCreateInfo->pBindings = textureBinding;
	globalLayoutCreateInfo->bindingCount = 1;

	return SDL_TRUE;
}


SDL_bool
GraphicsPipeline_DescriptorPoolCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_GraphicsPipelineCreateInfo* createInfo)
{
	///////////////
	/// aliases ///
	///////////////
	VkDescriptorSetLayoutCreateInfo* perSceneCreateInfo =
		&createInfo->subStructs->descriptorPoolCreateInfo.globalLayout;

	VkDescriptorSetLayout* perSceneLayout =
		&pipeline->subStructs->descriptorPool.globalLayout;

	//////////////////////
	/// create layouts ///
	//////////////////////

	VkResult err = vkCreateDescriptorSetLayout(
		renderer->subStructs->logicalDevice.device,
		perSceneCreateInfo,
		VSR_GetAllocator(),
		perSceneLayout
		);

	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create descriptor layout: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
		goto FAIL;
	}

	// size of big texture buffer no need for one per image as it read only
	VkDescriptorPoolSize poolSize[1];
	poolSize[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize[0].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolCreateInfo;
	poolCreateInfo.flags = 0L;
	poolCreateInfo.pNext = NULL;
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.maxSets = 1;
	poolCreateInfo.poolSizeCount = 1;
	poolCreateInfo.pPoolSizes = poolSize;

	err = vkCreateDescriptorPool(
		renderer->subStructs->logicalDevice.device,
		&poolCreateInfo,
		VSR_GetAllocator(),
		&pipeline->subStructs->descriptorPool.globalPool);

	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create descriptor pool: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
		goto FAIL;
	}

	VkDescriptorSetAllocateInfo allocateInfo;
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.pNext = NULL;
	allocateInfo.descriptorPool = pipeline->subStructs->descriptorPool.globalPool;
	allocateInfo.pSetLayouts = &pipeline->subStructs->descriptorPool.globalLayout;
	allocateInfo.descriptorSetCount = 1;

	err = vkAllocateDescriptorSets(
		renderer->subStructs->logicalDevice.device,
		&allocateInfo,
		&pipeline->subStructs->descriptorPool.globalSet
	);

	SUCCESS:
	return SDL_TRUE;
	FAIL:
	return SDL_FALSE;
}

void
GraphicsPipeline_DescriptorPoolDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline)
{

	vkDestroyDescriptorPool(
		renderer->subStructs->logicalDevice.device,
		pipeline->subStructs->descriptorPool.globalPool,
		VSR_GetAllocator()
	);

	vkDestroyDescriptorSetLayout(
		renderer->subStructs->logicalDevice.device,
		pipeline->subStructs->descriptorPool.globalLayout,
		VSR_GetAllocator()
	);

}
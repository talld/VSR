#include "Renderer_DescriptorPool.h"

#include "VSR_Renderer.h"
#include "VSR_GraphicsPipeline.h"
#include "VSR_error.h"

SDL_bool
Renderer_DescriptorPoolPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo)
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
	textureBinding->descriptorCount = renderer->subStructs->texturePoolSize;
	textureBinding->pImmutableSamplers = NULL;
	textureBinding->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	globalLayoutCreateInfo->sType =
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

	globalLayoutCreateInfo->flags = 0L;
	globalLayoutCreateInfo->pNext = NULL;

	globalLayoutCreateInfo->pBindings = textureBinding;
	globalLayoutCreateInfo->bindingCount = 1;

	return SDL_TRUE;
}


SDL_bool
Renderer_DescriptorPoolCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo * createInfo)
{
	///////////////
	/// aliases ///
	///////////////
	VkDescriptorSetLayoutCreateInfo* globalLayoutCreateInfo =
		&createInfo->subStructs->descriptorPoolCreateInfo.globalLayout;

	VkDescriptorSetLayout* globalLayout =
		&renderer->subStructs->descriptorPool.globalLayout;

	//////////////////////
	/// create layouts ///
	//////////////////////

	VkResult err = vkCreateDescriptorSetLayout(
		renderer->subStructs->logicalDevice.device,
		globalLayoutCreateInfo,
		VSR_GetAllocator(),
		globalLayout
		);

	if(err != VK_SUCCESS)
	{
        VSR_Error("Failed to create descriptor layout: %s",
				VSR_VkErrorToString(err));
		goto FAIL;
	}

	// size of big texture buffer no need for one per image as it read only
	VkDescriptorPoolSize poolSize[1];
	poolSize[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize[0].descriptorCount = renderer->subStructs->texturePoolSize;

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
		&renderer->subStructs->descriptorPool.globalPool);

	if(err != VK_SUCCESS)
	{
        VSR_Error("Failed to create descriptor pool: %s",
				VSR_VkErrorToString(err));
		goto FAIL;
	}

	VkDescriptorSetAllocateInfo allocateInfo;
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.pNext = NULL;
	allocateInfo.descriptorPool = renderer->subStructs->descriptorPool.globalPool;
	allocateInfo.pSetLayouts = globalLayout;
	allocateInfo.descriptorSetCount = 1;

	err = vkAllocateDescriptorSets(
		renderer->subStructs->logicalDevice.device,
		&allocateInfo,
		&renderer->subStructs->descriptorPool.globalSet
	);

	if(err != VK_SUCCESS)
	{
		VSR_Error("Failed to create descriptor pool: %s",
		          VSR_VkErrorToString(err));
		goto FAIL;
	}

	SUCCESS:
	return SDL_TRUE;
	FAIL:
	return SDL_FALSE;
}

void
Renderer_DescriptorPoolDestroy(
	VSR_Renderer* renderer)
{

	vkDestroyDescriptorPool(
		renderer->subStructs->logicalDevice.device,
		renderer->subStructs->descriptorPool.globalPool,
		VSR_GetAllocator()
	);

	vkDestroyDescriptorSetLayout(
		renderer->subStructs->logicalDevice.device,
		renderer->subStructs->descriptorPool.globalLayout,
		VSR_GetAllocator()
	);

}
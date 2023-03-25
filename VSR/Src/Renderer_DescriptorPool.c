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
		&createInfo->descriptorPoolCreateInfo.textureBinding;

	VkDescriptorSetLayoutCreateInfo* globalLayoutCreateInfo =
		&createInfo->descriptorPoolCreateInfo.globalLayout;

	VkDescriptorSetLayoutBinding* userBindings =
		createInfo->descriptorPoolCreateInfo.userBindings;

	VkDescriptorSetLayoutCreateInfo* userLayoutCreateInfo =
		&createInfo->descriptorPoolCreateInfo.userLayout;

	/////////////////////////////
	/// global descriptors ///
	/////////////////////////////
	textureBinding->binding = 0;
	textureBinding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureBinding->descriptorCount = renderer->texturePoolSize;
	textureBinding->pImmutableSamplers = NULL;
	textureBinding->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	globalLayoutCreateInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	globalLayoutCreateInfo->flags = 0L;
	globalLayoutCreateInfo->pNext = NULL;
	globalLayoutCreateInfo->pBindings = textureBinding;
	globalLayoutCreateInfo->bindingCount = 1;

	////////////////////////
	/// user descriptors ///
	////////////////////////
	size_t userDescriptorCount = renderer->extraDescriptorCount;
	for(size_t i = 0; i < userDescriptorCount; i++)
	{
		userBindings[i].binding = 0;
		userBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		userBindings[i].descriptorCount = 1;
		userBindings[i].pImmutableSamplers = NULL;
		userBindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	userLayoutCreateInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	userLayoutCreateInfo->flags = 0L;
	userLayoutCreateInfo->pNext = NULL;
	userLayoutCreateInfo->pBindings = userBindings;
	userLayoutCreateInfo->bindingCount = userDescriptorCount;

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
		&createInfo->descriptorPoolCreateInfo.globalLayout;

	VkDescriptorSetLayoutCreateInfo* userLayoutCreateInfo =
		&createInfo->descriptorPoolCreateInfo.userLayout;

	VkDescriptorSetLayout* globalLayout =
		&renderer->descriptorPool.globalLayout;

	VkDescriptorSetLayout* userLayout =
		&renderer->descriptorPool.userLayout;

	//////////////////////
	/// create layouts ///
	//////////////////////

	VkResult err = vkCreateDescriptorSetLayout(
		renderer->logicalDevice.device,
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

	err = vkCreateDescriptorSetLayout(
		renderer->logicalDevice.device,
		userLayoutCreateInfo,
		VSR_GetAllocator(),
		userLayout
	);

	if(err != VK_SUCCESS)
	{
		VSR_Error("Failed to create descriptor layout: %s",
		          VSR_VkErrorToString(err));
		goto FAIL;
	}

	// size of big texture buffer no need for one per image as it read only
	VkDescriptorPoolSize poolSizes[2];
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[0].descriptorCount = renderer->texturePoolSize;

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[1].descriptorCount = renderer->extraDescriptorCount;

	VkDescriptorPoolCreateInfo poolCreateInfo;
	poolCreateInfo.flags = 0L;
	poolCreateInfo.pNext = NULL;
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.maxSets = 1 + renderer->extraDescriptorCount + 1;
	poolCreateInfo.poolSizeCount = 1 + (renderer->extraDescriptorCount > 0);
	poolCreateInfo.pPoolSizes = poolSizes;

	err = vkCreateDescriptorPool(
		renderer->logicalDevice.device,
		&poolCreateInfo,
		VSR_GetAllocator(),
		&renderer->descriptorPool.globalPool);

	if(err != VK_SUCCESS)
	{
        VSR_Error("Failed to create descriptor pool: %s",
				VSR_VkErrorToString(err));
		goto FAIL;
	}

	VkDescriptorSetLayout layouts[2] = {*globalLayout, *userLayout};

	VkDescriptorSetAllocateInfo allocateInfo;
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.pNext = NULL;
	allocateInfo.descriptorPool = renderer->descriptorPool.globalPool;
	allocateInfo.descriptorSetCount = 2; //(renderer->extraDescriptorCount > 0);
	allocateInfo.pSetLayouts = layouts;

	VkDescriptorSet sets[2];


	err = vkAllocateDescriptorSets(
		renderer->logicalDevice.device,
		&allocateInfo,
		sets
	);

	if(err != VK_SUCCESS)
	{
		VSR_Error("Failed to create descriptor pool: %s",
		          VSR_VkErrorToString(err));
		goto FAIL;
	}

	renderer->descriptorPool.globalSet = sets[0];
	renderer->descriptorPool.userSet = sets[1];

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
		renderer->logicalDevice.device,
		renderer->descriptorPool.globalPool,
		VSR_GetAllocator()
	);

	vkDestroyDescriptorSetLayout(
		renderer->logicalDevice.device,
		renderer->descriptorPool.globalLayout,
		VSR_GetAllocator()
	);

	vkDestroyDescriptorSetLayout(
		renderer->logicalDevice.device,
		renderer->descriptorPool.userLayout,
		VSR_GetAllocator()
	);
}
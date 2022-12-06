#include "GraphicsPipeline_CommandPool.h"

#include "VSR_Renderer.h"
#include "VSR_error.h"





//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
GraphicsPipeline_CommandPoolPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo)
{
	return SDL_TRUE;
}





//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
GraphicsPipeline_CommandPoolCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_GraphicsPipelineCreateInfo* createInfo)
{
	/////////////////////
	/// command pools ///
	/////////////////////
	VkResult err;

	VkCommandPoolCreateInfo* poolCreateInfo =
		&createInfo->subStructs->commandPoolCreateInfo.commandPoolCreateInfo;

	poolCreateInfo->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo->pNext = NULL;
	poolCreateInfo->flags = 0L;


	poolCreateInfo->queueFamilyIndex = renderer->subStructs->deviceQueues.graphicsQueueFamilyIndex;
	err = vkCreateCommandPool(renderer->subStructs->logicalDevice.device,
						poolCreateInfo,
						VSR_GetAllocator(),
						&pipeline->subStructs->commandPool.graphicsPool);

	poolCreateInfo->queueFamilyIndex = renderer->subStructs->deviceQueues.transferQueueFamilyIndex;
	err = vkCreateCommandPool(renderer->subStructs->logicalDevice.device,
							  poolCreateInfo,
							  VSR_GetAllocator(),
							  &pipeline->subStructs->commandPool.transferPool);


	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create command pool: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
		goto FAIL;
	}


	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}

VkCommandBuffer
GraphicsPipeline_CommandPoolAllocateGraphicsBuffer(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline)
{
	///////////////////////
	/// command buffers ///
	///////////////////////
	VkCommandBuffer buff;

	VkCommandBufferAllocateInfo commandBufInfo = (VkCommandBufferAllocateInfo){0};
	commandBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufInfo.pNext = NULL;
	commandBufInfo.commandPool = pipeline->subStructs->commandPool.graphicsPool;
	commandBufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufInfo.commandBufferCount = 1;

	VkResult err = vkAllocateCommandBuffers(renderer->subStructs->logicalDevice.device,
								   &commandBufInfo,
								   &buff);

	if(err)
	{
		buff = NULL;
	}

	return buff;
}

VkCommandBuffer
GraphicsPipeline_CommandPoolAllocateTransferBuffer(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline)
{
	VkCommandBuffer buff;

	VkCommandBufferAllocateInfo allocInfo = (VkCommandBufferAllocateInfo){0};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = pipeline->subStructs->commandPool.transferPool;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(
		renderer->subStructs->logicalDevice.device,
							 &allocInfo,
							 &buff);

	return buff;
}

int
GraphicsPipeline_CommandBufferRecordStart(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VkCommandBuffer cBuff)
{
	VkCommandBufferBeginInfo bufferBeginInfo = (VkCommandBufferBeginInfo){0};
	bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	bufferBeginInfo.pNext = NULL;
	bufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	bufferBeginInfo.pInheritanceInfo = NULL;

	VkRenderPassBeginInfo passBeginInfo = (VkRenderPassBeginInfo){0};
	passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passBeginInfo.pNext = NULL;
	passBeginInfo.renderPass = pipeline->subStructs->renderPass.renderPass;
	passBeginInfo.framebuffer = pipeline->subStructs->framebuffer.framebuffers[renderer->subStructs->imageIndex];
	passBeginInfo.renderArea.offset.x = 0;
	passBeginInfo.renderArea.offset.y = 0;
	passBeginInfo.renderArea.extent.width = renderer->subStructs->surface.surfaceWidth;
	passBeginInfo.renderArea.extent.height = renderer->subStructs->surface.surfaceHeight;
	passBeginInfo.clearValueCount = 1;
	VkClearValue clearValues = {{{0.1f,0.1f,0.f, 1.0f}}};
	passBeginInfo.pClearValues = &clearValues;


	/////////////////////////
	/// record buffers    ///
	/////////////////////////
	{
		VkResult err = vkBeginCommandBuffer(cBuff, &bufferBeginInfo);
		if(err != VK_SUCCESS)
		{
			char errMsg[255];
			sprintf(errMsg, "Failed to start command recording: %s",
					VSR_VkErrorToString(err));

			VSR_SetErr(errMsg);
			goto FAIL;
		}
		vkCmdBeginRenderPass(cBuff, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cBuff,
						  VK_PIPELINE_BIND_POINT_GRAPHICS,
						  pipeline->subStructs->graphicPipeline.pipeline);

	}

	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}

int
GraphicsPipeline_CommandBufferRecordEnd(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VkCommandBuffer cBuff)
{

	///////////////////////////
	/// end record buffers ///
	//////////////////////////
	{
		vkCmdEndRenderPass(cBuff);

		VkResult err = vkEndCommandBuffer(cBuff);
		if (err != VK_SUCCESS)
		{
			char errMsg[255];
			sprintf(errMsg, "Failed to end command recording: %s",
					VSR_VkErrorToString(err));

			VSR_SetErr(errMsg);
			goto FAIL;
		}
	}

	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}

//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
void
GraphicsPipeline_CommandPoolDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline
)
{
	vkDestroyCommandPool(renderer->subStructs->logicalDevice.device,
						 pipeline->subStructs->commandPool.graphicsPool,
						 VSR_GetAllocator());

	vkDestroyCommandPool(renderer->subStructs->logicalDevice.device,
						 pipeline->subStructs->commandPool.transferPool,
						 VSR_GetAllocator());
}

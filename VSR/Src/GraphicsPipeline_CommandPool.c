#include "GraphicsPipeline_CommandPool.h"

#include "VSR_Renderer.h"
#include "VSR_error.h"





//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
Renderer_CommandPoolPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo)
{
	return SDL_TRUE;
}





//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
Renderer_CommandPoolCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo)
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


	poolCreateInfo->queueFamilyIndex = renderer->subStructs->deviceQueues.QFamilyIndexes[kGraphicsQueueIndex];
	err = vkCreateCommandPool(renderer->subStructs->logicalDevice.device,
						poolCreateInfo,
						VSR_GetAllocator(),
						&renderer->subStructs->commandPool.graphicsPool);

	poolCreateInfo->queueFamilyIndex = renderer->subStructs->deviceQueues.QFamilyIndexes[kTransferQueueIndex];
	err = vkCreateCommandPool(renderer->subStructs->logicalDevice.device,
							  poolCreateInfo,
							  VSR_GetAllocator(),
							  &renderer->subStructs->commandPool.transferPool);


	if(err != VK_SUCCESS)
	{
        VSR_Error("Failed to create command pool: %s",
				VSR_VkErrorToString(err));
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
Renderer_CommandPoolAllocateGraphicsBuffer(
	VSR_Renderer* renderer)
{
	///////////////////////
	/// command buffers ///
	///////////////////////
	VkCommandBuffer buff;

	VkCommandBufferAllocateInfo commandBufInfo = (VkCommandBufferAllocateInfo){0};
	commandBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufInfo.pNext = NULL;
	commandBufInfo.commandPool = renderer->subStructs->commandPool.graphicsPool;
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
Renderer_CommandPoolAllocateTransferBuffer(
	VSR_Renderer* renderer)
{
	VkCommandBuffer buff;

	VkCommandBufferAllocateInfo allocInfo = (VkCommandBufferAllocateInfo){0};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = renderer->subStructs->commandPool.transferPool;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(
		renderer->subStructs->logicalDevice.device,
							 &allocInfo,
							 &buff);

	VkCommandBufferBeginInfo beginInfo = (VkCommandBufferBeginInfo){0};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(buff, &beginInfo);

	return buff;
}

void
Renderer_CommandPoolSubmitTransferBuffer(
	VSR_Renderer* renderer,
	VkCommandBuffer buff)
{
	vkEndCommandBuffer(buff);

	VkSubmitInfo submitInfo = (VkSubmitInfo){0};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buff;

	VkFence transferFence;
	VkFenceCreateInfo fenceInfo;
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = NULL;
	fenceInfo.flags = 0;

	vkCreateFence(
		renderer->subStructs->logicalDevice.device,
		&fenceInfo,
		VSR_GetAllocator(),
		&transferFence);

	vkQueueSubmit(
		renderer->subStructs->deviceQueues.QList[kTransferQueueIndex],
		1,
		&submitInfo,
		transferFence);

	vkWaitForFences(
		renderer->subStructs->logicalDevice.device,
		1,
		&transferFence,
		VK_TRUE,
		-1);

	vkDestroyFence(
		renderer->subStructs->logicalDevice.device,
		transferFence,
		VSR_GetAllocator());
}

int
Renderer_CommandBufferRecordStart(
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
	passBeginInfo.clearValueCount = 2;
	VkClearValue clearValues[2] = {
		{ .color = {{0.0f,0.0f,0.f, 1.0f}} },
		{ .depthStencil = {1.0f} }
	};
	passBeginInfo.pClearValues = clearValues;


	/////////////////////////
	/// record buffers    ///
	/////////////////////////
	{
		VkResult err = vkBeginCommandBuffer(cBuff, &bufferBeginInfo);
		if(err != VK_SUCCESS)
		{
            VSR_Error("Failed to start command recording: %s",
					VSR_VkErrorToString(err));
			goto FAIL;
		}
		vkCmdBeginRenderPass(cBuff, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cBuff,
						  VK_PIPELINE_BIND_POINT_GRAPHICS,
						  pipeline->subStructs->graphicPipeline.pipeline);

		vkCmdBindDescriptorSets(
			cBuff,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline->subStructs->graphicPipeline
			.pipelineLayout,0,
			1, &renderer->subStructs->descriptorPool.globalSet,
			0 ,NULL
		);

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
Renderer_CommandBufferRecordEnd(
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
            VSR_Error("Failed to end command recording: %s",
					VSR_VkErrorToString(err));
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
Renderer_CommandPoolDestroy(
	VSR_Renderer* renderer)
{
	vkDestroyCommandPool(renderer->subStructs->logicalDevice.device,
						 renderer->subStructs->commandPool.graphicsPool,
						 VSR_GetAllocator());

	vkDestroyCommandPool(renderer->subStructs->logicalDevice.device,
	                     renderer->subStructs->commandPool.transferPool,
						 VSR_GetAllocator());
}

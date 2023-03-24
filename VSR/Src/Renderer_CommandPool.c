#include "Renderer_CommandPool.h"

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
		&createInfo->commandPoolCreateInfo.commandPoolCreateInfo;

	poolCreateInfo->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo->pNext = NULL;
	poolCreateInfo->flags = 0L;


	poolCreateInfo->queueFamilyIndex = renderer->deviceQueues.QFamilyIndexes[kGraphicsQueueIndex];
	err = vkCreateCommandPool(renderer->logicalDevice.device,
						poolCreateInfo,
						VSR_GetAllocator(),
						&renderer->commandPool.graphicsPool);

	poolCreateInfo->queueFamilyIndex = renderer->deviceQueues.QFamilyIndexes[kTransferQueueIndex];
	err = vkCreateCommandPool(renderer->logicalDevice.device,
							  poolCreateInfo,
							  VSR_GetAllocator(),
							  &renderer->commandPool.transferPool);


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
	commandBufInfo.commandPool = renderer->commandPool.graphicsPool;
	commandBufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufInfo.commandBufferCount = 1;

	VkResult err = vkAllocateCommandBuffers(renderer->logicalDevice.device,
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
	allocInfo.commandPool = renderer->commandPool.transferPool;
	allocInfo.commandBufferCount = 1;

	VkResult err = vkAllocateCommandBuffers(
		renderer->logicalDevice.device,
		&allocInfo,
		&buff);

	if(err == VK_SUCCESS)
	{
		VkCommandBufferBeginInfo beginInfo = (VkCommandBufferBeginInfo) {0};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(buff, &beginInfo);
	}
	else
	{
		buff = NULL;
	}

	return buff;
}

void
Renderer_CommandPoolSubmitTransferBuffer(
	VSR_Renderer* renderer,
	VkCommandBuffer buff,
	VkFence fence)
{
	vkEndCommandBuffer(buff);

	SDL_bool bInternalSync = SDL_FALSE;

	if(fence)
	{
		vkResetFences(
			renderer->logicalDevice.device,
			1,
			&fence
		);
	}
	else
	{
		bInternalSync = SDL_TRUE;

		VkFenceCreateInfo fenceCreateInfo = (VkFenceCreateInfo){0};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = NULL;
		fenceCreateInfo.flags = 0L;

		vkCreateFence(
			renderer->logicalDevice.device,
			&fenceCreateInfo,
			VSR_GetAllocator(),
			&fence
		);
	}

	VkSubmitInfo submitInfo = (VkSubmitInfo){0};
	submitInfo.pNext = NULL;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buff;

	vkQueueSubmit(
		renderer->deviceQueues.QList[kTransferQueueIndex],
		1,
		&submitInfo,
		fence
	);

	if(bInternalSync)
	{
		vkWaitForFences(
			renderer->logicalDevice.device,
			1,
			&fence,
			VK_TRUE,
			-1
		);

		vkDestroyFence(
			renderer->logicalDevice.device,
			fence,
			VSR_GetAllocator()
		);
	}
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
	passBeginInfo.renderPass = pipeline->renderPass.renderPass;
	passBeginInfo.framebuffer = pipeline->framebuffer.framebuffers[renderer->imageIndex];
	passBeginInfo.renderArea.offset.x = 0;
	passBeginInfo.renderArea.offset.y = 0;
	passBeginInfo.renderArea.extent.width = renderer->surface.surfaceWidth;
	passBeginInfo.renderArea.extent.height = renderer->surface.surfaceHeight;
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
						  pipeline->graphicPipeline.pipeline);

		vkCmdBindDescriptorSets(
			cBuff,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline->graphicPipeline
			.pipelineLayout,0,
			1, &renderer->descriptorPool.globalSet,
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
	vkDestroyCommandPool(renderer->logicalDevice.device,
						 renderer->commandPool.graphicsPool,
						 VSR_GetAllocator());

	vkDestroyCommandPool(renderer->logicalDevice.device,
	                     renderer->commandPool.transferPool,
						 VSR_GetAllocator());
}

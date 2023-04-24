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

	/// pass settings over ///
	renderer->commandPool.cmdBuffersPerPool = createInfo->cmdBuffersPerPool;

	/// ///
	VkCommandPoolCreateInfo* poolCreateInfo =
		&createInfo->commandPoolCreateInfo.commandPoolCreateInfo;

	poolCreateInfo->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo->pNext = NULL;
	poolCreateInfo->flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

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

	////////////////////////////////
	/// allocate synchronization ///
	////////////////////////////////

	VkFenceCreateInfo fenceCreateInfo;
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = NULL;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	size_t fenceListSize = renderer->commandPool.cmdBuffersPerPool * sizeof(VkFence);
	renderer->commandPool.graphicsCmdReadySignals = SDL_malloc(fenceListSize);
	renderer->commandPool.transferCmdReadySignals = SDL_malloc(fenceListSize);

	size_t generationListSize = renderer->commandPool.cmdBuffersPerPool * sizeof(size_t);
	renderer->commandPool.graphicsCmdReadySignalsGeneration = SDL_malloc(generationListSize);
	renderer->commandPool.transferCmdReadySignalsGeneration = SDL_malloc(generationListSize);

	for(size_t i = 0; i < renderer->commandPool.cmdBuffersPerPool; i++)
	{
		vkCreateFence(
			renderer->logicalDevice.device,
			&fenceCreateInfo,
			VSR_GetAllocator(),
			&renderer->commandPool.graphicsCmdReadySignals[i]
		);

		vkCreateFence(
			renderer->logicalDevice.device,
			&fenceCreateInfo,
			VSR_GetAllocator(),
			&renderer->commandPool.transferCmdReadySignals[i]
		);
	}

	////////////////////////////////
	/// allocate command buffers ///
	////////////////////////////////

	VkCommandBufferAllocateInfo commandBuffAllocateInfo = (VkCommandBufferAllocateInfo){0};
	commandBuffAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBuffAllocateInfo.pNext = NULL;
	commandBuffAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBuffAllocateInfo.commandBufferCount = renderer->commandPool.cmdBuffersPerPool;


	/// graphics pool ///
	commandBuffAllocateInfo.commandPool = renderer->commandPool.graphicsPool;

	renderer->commandPool.graphicsCmdBuffers = SDL_malloc(
		renderer->commandPool.cmdBuffersPerPool * sizeof(VkCommandBuffer));

	err = vkAllocateCommandBuffers(
		renderer->logicalDevice.device,
		&commandBuffAllocateInfo,
		renderer->commandPool.graphicsCmdBuffers);

	if(err != VK_SUCCESS)
	{
		VSR_Error("Failed to create command pool: %s",
		          VSR_VkErrorToString(err));
		goto FAIL;
	}


	/// transfer pool ///
	commandBuffAllocateInfo.commandPool = renderer->commandPool.transferPool;

	renderer->commandPool.transferCmdBuffers = SDL_malloc(
		renderer->commandPool.cmdBuffersPerPool * sizeof(VkCommandBuffer));

	err = vkAllocateCommandBuffers(
		renderer->logicalDevice.device,
		&commandBuffAllocateInfo,
		renderer->commandPool.transferCmdBuffers);


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
	VSR_Renderer* renderer,
	VSR_GenerationalFence * fence)
{

	size_t readyIndex = -1;
	do
	{

		for (size_t i = 0; i < renderer->commandPool.cmdBuffersPerPool; i++)
		{
			VkResult res = vkGetFenceStatus(
				renderer->logicalDevice.device,
				renderer->commandPool.graphicsCmdReadySignals[i]
			);

			if (res == VK_SUCCESS)
			{
				readyIndex = i;
				break;
			}
		}

		if(readyIndex == -1)
		{
			vkWaitForFences(
				renderer->logicalDevice.device,
				renderer->commandPool.cmdBuffersPerPool,
				renderer->commandPool.graphicsCmdReadySignals,
				VK_FALSE,
				-1
			);
		}

	} while (readyIndex == -1);

	VkCommandBuffer buff = renderer->commandPool.graphicsCmdBuffers[readyIndex];
	VkFence buffFence = renderer->commandPool.graphicsCmdReadySignals[readyIndex];

	VkCommandBufferResetFlags resetFlags = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;
	vkResetCommandBuffer(buff, resetFlags);

	vkResetFences(
		renderer->logicalDevice.device,
		1,
		&buffFence
	);

	size_t* generation = &renderer->commandPool.graphicsCmdReadySignalsGeneration[readyIndex];
	*generation++;
	if(fence)
	{
		*fence = (VSR_GenerationalFence){.fence = buffFence, .generation = generation};
	}

	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = NULL;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = NULL;

	vkBeginCommandBuffer(buff, &beginInfo);
	return buff;
}

VkCommandBuffer
Renderer_CommandPoolAllocateTransferBuffer(
	VSR_Renderer* renderer,
	VSR_GenerationalFence* fence)
{
	size_t readyIndex = -1;
	do
	{

		for (size_t i = 0; i < renderer->commandPool.cmdBuffersPerPool; i++)
		{
			VkResult res = vkGetFenceStatus(
				renderer->logicalDevice.device,
				renderer->commandPool.transferCmdReadySignals[i]
			);

			if (res == VK_SUCCESS)
			{
				readyIndex = i;
				break;
			}
		}

		if(readyIndex == -1)
		{
			vkWaitForFences(
				renderer->logicalDevice.device,
				renderer->commandPool.cmdBuffersPerPool,
				renderer->commandPool.transferCmdReadySignals,
				VK_FALSE,
				-1
			);
		}

	} while (readyIndex == -1);

	VkCommandBuffer buff = renderer->commandPool.transferCmdBuffers[readyIndex];
	VkFence buffFence = renderer->commandPool.transferCmdReadySignals[readyIndex];

	VkCommandBufferResetFlags resetFlags = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;
	vkResetCommandBuffer(buff, resetFlags);

	vkResetFences(
		renderer->logicalDevice.device,
		1,
		&buffFence
	);

	size_t* generation = &renderer->commandPool.transferCmdReadySignalsGeneration[readyIndex];
	*generation++;
	if(fence)
	{
		*fence = (VSR_GenerationalFence){.fence = buffFence, .generation = generation};
	}

	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = NULL;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = NULL;

	vkBeginCommandBuffer(buff, &beginInfo);
	return buff;
}

void
Renderer_CommandPoolSubmitTransferBuffer(
	VSR_Renderer* renderer,
	VkCommandBuffer buff)
{
	VkFence cmdFence;

	for (size_t i = 0; i < renderer->commandPool.cmdBuffersPerPool; i++)
	{
		if (buff == renderer->commandPool.transferCmdBuffers[i])
		{
			cmdFence = renderer->commandPool.transferCmdReadySignals[i];
		}
	}

	vkEndCommandBuffer(buff);

	VkSubmitInfo submitInfo = (VkSubmitInfo){0};
	submitInfo.pNext = NULL;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buff;

	vkQueueSubmit(
		renderer->deviceQueues.QList[kTransferQueueIndex],
		1,
		&submitInfo,
		cmdFence
	);
}

int
Renderer_CommandBufferRecordStart(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VkCommandBuffer cBuff)
{
	VkRenderPassBeginInfo passBeginInfo = (VkRenderPassBeginInfo){0};
	passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passBeginInfo.pNext = NULL;
	passBeginInfo.renderPass = renderer->renderPass.renderPass;
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

	if(renderer->pipeline->renderTarget == NULL)
	{
		passBeginInfo.framebuffer =
			Renderer_GetSwapchainFrames(renderer)[renderer->imageIndex]->frame;
	}
	else
	{
		passBeginInfo.framebuffer = renderer->pipeline->renderTarget->framebuffer->frame;
	}
	/////////////////////////
	/// record buffers    ///
	/////////////////////////
	{
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
	for(size_t i = 0; i < renderer->commandPool.cmdBuffersPerPool; i++)
	{
		vkDestroyFence(
			renderer->logicalDevice.device,
			renderer->commandPool.graphicsCmdReadySignals[i],
			VSR_GetAllocator()
		);

		vkDestroyFence(
			renderer->logicalDevice.device,
			renderer->commandPool.transferCmdReadySignals[i],
			VSR_GetAllocator()
		);
	}

	vkDestroyCommandPool(renderer->logicalDevice.device,
						 renderer->commandPool.graphicsPool,
						 VSR_GetAllocator());

	vkDestroyCommandPool(renderer->logicalDevice.device,
	                     renderer->commandPool.transferPool,
						 VSR_GetAllocator());
}

#include "Renderer_CommandPool.h"

#include "Renderer.h"
#include "VSR_error.h"





//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_CommandPoolPopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs)
{
	return SDL_TRUE;
}





//==============================================================================
// VSR_SwapchainPopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_CommandPoolCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs)
{
	/////////////////////
	/// command pools ///
	/////////////////////

	VkCommandPoolCreateInfo* poolCreateInfo =
		&subStructs->commandPoolCreateInfo.commandPoolCreateInfo;

	poolCreateInfo->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo->pNext = NULL;
	poolCreateInfo->flags = 0L;


	poolCreateInfo->queueFamilyIndex = renderer->subStructs->deviceQueues.graphicsQueueFamilyIndex;
	VkResult err;
	err = vkCreateCommandPool(renderer->subStructs->logicalDevice.device,
						poolCreateInfo,
						VSR_GetAllocator(),
						&renderer->subStructs->commandPool.graphicsPool);

	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create command pool: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
		goto FAIL;
	}

	///////////////////////
	/// command buffers ///
	///////////////////////



	size_t frames = renderer->subStructs->swapchain.imageViewCount;
	size_t listSize = frames * sizeof(VkCommandBuffer);
	renderer->subStructs->commandPool.commandBuffers = SDL_malloc(listSize);
	VkCommandBuffer* buffs =  renderer->subStructs->commandPool.commandBuffers;

	VkCommandBufferAllocateInfo commandBufInfo = (VkCommandBufferAllocateInfo){0};
	commandBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufInfo.pNext = NULL;
	commandBufInfo.commandPool = renderer->subStructs->commandPool.graphicsPool;
	commandBufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufInfo.commandBufferCount = frames;

	err = vkAllocateCommandBuffers(renderer->subStructs->logicalDevice.device,
								   &commandBufInfo,
								   buffs);

	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create command buffers: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
		goto FAIL;
	}

	/////////////////////////
	/// prerecord buffers ///
	/////////////////////////

	VkCommandBufferBeginInfo bufferBeginInfo;
	bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	bufferBeginInfo.pNext = NULL;
	bufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	bufferBeginInfo.pInheritanceInfo = NULL;

	VkRenderPassBeginInfo passBeginInfo;
	passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passBeginInfo.pNext = NULL;
	passBeginInfo.renderPass = renderer->subStructs->renderPass.renderPass;
	passBeginInfo.renderArea.offset.x = 0;
	passBeginInfo.renderArea.offset.y = 0;
	passBeginInfo.renderArea.extent.width = renderer->subStructs->surface.surfaceWidth;
	passBeginInfo.renderArea.extent.height = renderer->subStructs->surface.surfaceHeight;
	passBeginInfo.clearValueCount = 0;
	passBeginInfo.pClearValues = NULL;

	for(size_t i = 0; i < frames; i++)
	{
		vkBeginCommandBuffer(buffs[i], &bufferBeginInfo);
		if(err != VK_SUCCESS)
		{
			char errMsg[255];
			sprintf(errMsg, "Failed to start command recording: %s",
					VSR_VkErrorToString(err));

			VSR_SetErr(errMsg);
			goto FAIL;
		}

		passBeginInfo.framebuffer = renderer->subStructs->framebuffer.framebuffers[i];
		vkCmdBeginRenderPass(buffs[i], &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		{
			// colour + depth pass?
			vkCmdBindPipeline(buffs[i],
							  VK_PIPELINE_BIND_POINT_GRAPHICS,
							  renderer->subStructs->graphicPipeline.pipeline);

			vkCmdDraw(buffs[i], 3, 1, 0, 0);

			// vkCmdDraw(buffs[i], verts, instances, 0, 0);
			// vkCmdPushConstants(buffs[i]... camera constants ...);
		}
		vkCmdEndRenderPass(buffs[i]);

		vkEndCommandBuffer(buffs[i]);
		if(err != VK_SUCCESS)
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
VSR_CommandPoolDestroy(
	VSR_Renderer* renderer
)
{
	vkDestroyCommandPool(renderer->subStructs->logicalDevice.device,
						 renderer->subStructs->commandPool.graphicsPool,
						 VSR_GetAllocator());
}
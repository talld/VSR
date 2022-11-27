#include "Renderer.h"

#include <vulkan/vulkan.h>
#include "VSR_error.h"





//==============================================================================
// Renderer_CreateSyncObjects
//------------------------------------------------------------------------------
void Renderer_CreateSyncObjects(VSR_Renderer* renderer)
{
	//////////////////////////////////
	/// alloc list of sync objects ///
	//////////////////////////////////
	size_t listSize;

	listSize = renderer->subStructs->swapchain.imageViewCount * sizeof(VkSemaphore);
	renderer->subStructs->imageCanBeRead = SDL_malloc(listSize);
	renderer->subStructs->imageCanBeWritten = SDL_malloc(listSize);

	listSize = renderer->subStructs->swapchain.imageViewCount * sizeof(VkFence);
	renderer->subStructs->imageFinished = SDL_malloc(listSize);

	//////////////////////////////
	/// Create semaphores info ///
	//////////////////////////////
	VkSemaphoreCreateInfo semaphoreInfo = (VkSemaphoreCreateInfo){0};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.pNext = NULL;
	semaphoreInfo.flags = 0L;

	//////////////////////////
	/// Create fences info ///
	//////////////////////////

	VkFenceCreateInfo fenceInfo = (VkFenceCreateInfo){0};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = NULL;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(size_t i = 0; i < renderer->subStructs->swapchain.imageViewCount; i++)
	{
		vkCreateSemaphore(renderer->subStructs->logicalDevice.device,
						  &semaphoreInfo,
						  VSR_GetAllocator(),
						  &renderer->subStructs->imageCanBeRead[i]);

		vkCreateSemaphore(renderer->subStructs->logicalDevice.device,
						  &semaphoreInfo,
						  VSR_GetAllocator(),
						  &renderer->subStructs->imageCanBeWritten[i]);

		vkCreateFence(renderer->subStructs->logicalDevice.device,
					  &fenceInfo,
					  VSR_GetAllocator(),
					  &renderer->subStructs->imageFinished[i]);
	}

}





//==============================================================================
// Renderer_DestroySyncObjects
//------------------------------------------------------------------------------
void Renderer_DestroySyncObjects(VSR_Renderer* renderer)
{
	for(size_t i = 0; i < renderer->subStructs->swapchain.imageViewCount; i++)
	{
		vkDestroySemaphore(renderer->subStructs->logicalDevice.device,
						   renderer->subStructs->imageCanBeWritten[i],
						   VSR_GetAllocator());

		vkDestroySemaphore(renderer->subStructs->logicalDevice.device,
						   renderer->subStructs->imageCanBeRead[i],
						   VSR_GetAllocator());

		vkDestroyFence(renderer->subStructs->logicalDevice.device,
						  renderer->subStructs->imageFinished[i],
						  VSR_GetAllocator());
	}

	SDL_free(renderer->subStructs->imageCanBeWritten);
	SDL_free(renderer->subStructs->imageCanBeRead);
	SDL_free(renderer->subStructs->imageFinished);

}





//==============================================================================
// VSR_RendererGenerateCreateInfo
//------------------------------------------------------------------------------
VSR_RendererCreateInfo*
VSR_RendererGenerateCreateInfo(
	SDL_Window* window,
	VSR_CreateInfoFlags flags)
{
	// TODO: at a later stage function call for vulkan should be gotten by
	// pulling functions pointers from these functions
	// vkGetInstanceProcAddr = SDL_Vulkan_GetVkGetInstanceProcAddr();
	// fn = vkGetInstanceProcAddr(VkInstance instance, const char* name);
	// fn(args);
	//
	// this is to ensure that SDL and VSR are using the same version of
	// vulkan and they're function calls will always do the same thing
	
	////////////////////////
	/// allocate structs ///
	////////////////////////
	size_t createInfoSize = sizeof(VSR_RendererCreateInfo);
	VSR_RendererCreateInfo* createInfo =  SDL_calloc(1, createInfoSize);
	
	size_t subStructsSize = sizeof(VSR_RendererCreateInfoSubStructs);
	createInfo->subStructs = SDL_calloc(1, subStructsSize);

	createInfo->SDLWindow = window;

	////////////////////////////
	/// Process passed flags ///
	////////////////////////////
	if(flags & VSR_CREATE_INFO_GEOMETRY_SHADER)
	{
		createInfo->geometryShaderRequested = SDL_TRUE;
	}
	
	if(flags & VSR_CREATE_INFO_TESSELATION_SHADER)
	{
		createInfo->tessellationShaderRequested = SDL_TRUE;
	}

	////////////////////////////////////////////////////////////////////////////
	/// populate vk create info structs as much as can be done at the moment ///
	////////////////////////////////////////////////////////////////////////////
	VSR_InstancePopulateCreateInfo(createInfo, createInfo->subStructs);
	VSR_DeviceQueuesPopulateCreateInfo(createInfo, createInfo->subStructs);
    VSR_LogicalDevicePopulateCreateInfo(createInfo, createInfo->subStructs);
	VSR_SwapchainPopulateCreateInfo(createInfo, createInfo->subStructs);
	VSR_RenderPassPopulateCreateInfo(createInfo, createInfo->subStructs);
	VSR_GraphicsPipelinePopulateCreateInfo(createInfo, createInfo->subStructs);
	VSR_FramebufferPopulateCreateInfo(createInfo, createInfo->subStructs);
	VSR_CommandPoolPopulateCreateInfo(createInfo, createInfo->subStructs);

	SUCCESS:
	{
		return createInfo;
	}
	
	FAIL:
	{
		return NULL;
	}
}





//==============================================================================
// VSR_RendererFreeCreateInfo
//------------------------------------------------------------------------------
void
VSR_RendererFreeCreateInfo(
	VSR_RendererCreateInfo* rendererCreateInfo)
{
	VSR_RendererCreateInfoSubStructs* subStructs =
		rendererCreateInfo->subStructs;
	
	// in an ideal world all of this would not exist
	// all the memory needed should be part of the VSR_RendererCreateInfo struct
	// work should be done to minimise malloc wherever possible
	
	///////////////////////////////////////////
	/// Free vkStructs create info pointers ///
	///////////////////////////////////////////
	SDL_free((void*)subStructs);
	
	///////////////////////////////////
	/// free the rendererCreateInfo ///
	///////////////////////////////////
	SDL_free((void*)rendererCreateInfo);
}





//==============================================================================
// VSR_CreateRenderer
//------------------------------------------------------------------------------
VSR_Renderer*
VSR_RendererCreate(
	VSR_RendererCreateInfo* rendererCreateInfo)
{
	VSR_Renderer* renderer = SDL_calloc(1, sizeof(VSR_Renderer));
	renderer->subStructs = SDL_calloc(1, sizeof(VSR_RendererSubStructs));
	
	//////////////////////////////////
	/// pass info to new structure ///
	//////////////////////////////////
	renderer->SDLWindow = rendererCreateInfo->SDLWindow;
	
	VSR_InstanceCreate(renderer, rendererCreateInfo->subStructs);
	VSR_SurfaceCreate(renderer, rendererCreateInfo->subStructs);
	VSR_PhysicalDeviceSelect(renderer, rendererCreateInfo->subStructs);
	VSR_DeviceQueuesCreate(renderer, rendererCreateInfo->subStructs);
	VSR_LogicalDeviceCreate(renderer, rendererCreateInfo->subStructs);

	// TODO: move this to its own VSR_GraphicsPipeline struct
	VSR_SwapchainCreate(renderer, rendererCreateInfo->subStructs);

	VSR_RenderPassCreate(renderer, rendererCreateInfo->subStructs);
	VSR_GraphicsPipelineCreate(renderer, rendererCreateInfo->subStructs);
	VSR_FramebufferCreate(renderer, rendererCreateInfo->subStructs);
	VSR_CommandPoolCreate(renderer, rendererCreateInfo->subStructs);

	Renderer_CreateSyncObjects(renderer);

	return renderer;
}





//==============================================================================
// VSR_FreeRenderer
//------------------------------------------------------------------------------
void
VSR_RendererFree(
	VSR_Renderer* renderer)
{
	////////////////////////////////////////
	/// wait for rendering to power down ///
	////////////////////////////////////////
	// TODO: make multi-threading safe
	vkDeviceWaitIdle(renderer->subStructs->logicalDevice.device);

	////////////////////////////////////////
	/// Destroy VkStructs Vulkan objects ///
	////////////////////////////////////////
	Renderer_DestroySyncObjects(renderer);

	VSR_CommandPoolDestroy(renderer);
	VSR_FramebufferDestroy(renderer);
	VSR_GraphicPipelineDestroy(renderer);
	VSR_RenderPassDestroy(renderer);
	VSR_SwapchainDestroy(renderer);
	VSR_LogicalDeviceDestroy(renderer);
	VSR_SurfaceDestroy(renderer);
	VSR_InstanceDestroy(renderer);

	/////////////////////////////////
	/// Free renderer's VKStructs ///
	/////////////////////////////////
	SDL_free((void*)renderer->subStructs);
	
	/////////////////////
	/// Free renderer ///
	/////////////////////
	SDL_free((void*)renderer);
}





//==============================================================================
// VSR_RendererSetShader
//------------------------------------------------------------------------------
void
VSR_RendererSetShader(
	VSR_Renderer* renderer,
	VSR_ShaderStage stage,
	VSR_Shader* shader)
{
	if(stage == SHADER_STAGE_FRAGMENT)
	{
		renderer->subStructs->fragmentShader = shader;
	}

	if(stage == SHADER_STAGE_VERTEX)
	{
		renderer->subStructs->vertexShader = shader;
	}

}





//==============================================================================
// VSR_RendererBeginPass
//------------------------------------------------------------------------------
void VSR_RendererBeginPass(VSR_Renderer* renderer)
{
	///////////////
	/// aliases ///
	///////////////
	size_t* frameIndex = &renderer->subStructs->currentFrame;


	////////////////////////////////////////////////////////////////
	/// wait for requested image to be done ( ready to use again ///
	////////////////////////////////////////////////////////////////
	vkWaitForFences(renderer->subStructs->logicalDevice.device,
					1,
					&renderer->subStructs->imageFinished[*frameIndex],
					VK_TRUE,
					-1);

	vkResetFences(renderer->subStructs->logicalDevice.device,
				  1,
				  &renderer->subStructs->imageFinished[*frameIndex]);


	///////////////////////////
	/// get swapchain image ///
	///////////////////////////

	uint32_t imageIndex;
	vkAcquireNextImageKHR(renderer->subStructs->logicalDevice.device,
						  renderer->subStructs->swapchain.swapchain,
						  -1,
						  renderer->subStructs->imageCanBeWritten[*frameIndex],
						  VK_NULL_HANDLE,
						  &imageIndex);

	////////////////////////////////
	/// submit commands to queue ///
	////////////////////////////////
	VkPipelineStageFlags waitStages[1] =
		{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	VkSubmitInfo submitInfo = (VkSubmitInfo){0};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &renderer->subStructs->imageCanBeWritten[*frameIndex];
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderer->subStructs->imageCanBeRead[*frameIndex];
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &renderer->subStructs->commandPool.commandBuffers[*frameIndex];

	vkQueueSubmit(renderer->subStructs->deviceQueues.graphicsQueue,
				  1,
				  &submitInfo,
				  renderer->subStructs->imageFinished[*frameIndex]);

	///////////////////////////////////////////
	/// present queue information to screen ///
	///////////////////////////////////////////
	VkPresentInfoKHR presentInfo = (VkPresentInfoKHR){0};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderer->subStructs->imageCanBeRead[*frameIndex];
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &renderer->subStructs->swapchain.swapchain;
	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(renderer->subStructs->deviceQueues.presentQueue,
					  &presentInfo);

	*frameIndex = (*frameIndex + 1) % renderer->subStructs->swapchain.imageViewCount;
}





//==============================================================================
// VSR_RendererEndPass
//------------------------------------------------------------------------------
void VSR_RendererEndPass(VSR_Renderer* renderer)
{

}
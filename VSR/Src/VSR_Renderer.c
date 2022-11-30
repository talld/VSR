#include "VSR_Renderer.h"

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
	
	size_t subStructsSize = sizeof(Renderer_CreateInfoSubStructs);
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
	Renderer_CreateInfoSubStructs* subStructs =
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
	renderer->subStructs = SDL_calloc(1, sizeof(Renderer_SubStructs));
	
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

void VSR_RendererSetPipeline(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline)
{
	renderer->subStructs->pipeline = pipeline;
}


//==============================================================================
// VSR_RendererBeginPass
//------------------------------------------------------------------------------
void VSR_RendererBeginPass(VSR_Renderer* renderer)
{
	////////////////
	/// Bouncers ///
	////////////////
	if(!renderer->subStructs->pipeline) return;

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

	//////////////////
	/// reset sync ///
	//////////////////
	vkResetFences(renderer->subStructs->logicalDevice.device,
				  1,
				  &renderer->subStructs->imageFinished[*frameIndex]);

	///////////////////////////
	/// get swapchain image ///
	///////////////////////////
	vkAcquireNextImageKHR(renderer->subStructs->logicalDevice.device,
						  renderer->subStructs->swapchain.swapchain,
						  -1,
						  renderer->subStructs->imageCanBeWritten[*frameIndex],
						  VK_NULL_HANDLE,
						  &renderer->subStructs->imageIndex);

	/// step 1 command record
	GraphicsPipeline_CommandBufferRecordStart(
		renderer,
		renderer->subStructs->pipeline);
}





//==============================================================================
// VSR_RendererEndPass
//------------------------------------------------------------------------------
void VSR_RendererEndPass(VSR_Renderer* renderer)
{
	////////////////
	/// Bouncers ///
	////////////////
	if(!renderer->subStructs->pipeline) return;

	///////////////
	/// aliases ///
	///////////////
	size_t* frameIndex = &renderer->subStructs->currentFrame;


	/// step2 command record
	GraphicsPipeline_CommandBufferRecordEnd(
		renderer,
		renderer->subStructs->pipeline);

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
	submitInfo.pCommandBuffers = &renderer->subStructs->pipeline->
		subStructs->commandPool.commandBuffers[*frameIndex];

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
	presentInfo.pImageIndices = &renderer->subStructs->imageIndex;

	vkQueuePresentKHR(renderer->subStructs->deviceQueues.presentQueue,
					  &presentInfo);

	*frameIndex = (*frameIndex + 1) % renderer->subStructs->swapchain.imageViewCount;
}


void Renderer_GrowMemory(VSR_Renderer* renderer, size_t growAmount)
{
	size_t newSize = renderer->subStructs->deviceBufferMemorySize + growAmount;
	const VkBufferUsageFlagBits use = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;


	VkBufferCreateInfo bufferInfo = (VkBufferCreateInfo){0};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = NULL;
	bufferInfo.flags = 0L;
	bufferInfo.size = newSize;
	bufferInfo.usage = use;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkDeviceMemory mem =
		VSR_LogicalDeviceGetMemory(renderer,
								   newSize,
								   bufferInfo.usage,
								   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
								   | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
								   &renderer->subStructs->deviceBuffer);

	renderer->subStructs->deviceBufferMemorySize = newSize;
	renderer->subStructs->deviceBufferMemory = mem;
}


//==============================================================================
// Renderer_AllocateModelBuffer
//------------------------------------------------------------------------------
Renderer_ModelBuffer
Renderer_AllocateModelBuffer(
	VSR_Renderer* renderer,
	VSR_Model* model,
	Renderer_ResourceType resourceType,
	size_t size)
{
	Renderer_ModelBuffer modelBuffer = (Renderer_ModelBuffer){};

	if(!renderer->subStructs->deviceBufferMemorySize)
	{
		Renderer_GrowMemory(renderer, size);
	}


	static size_t offset = 0;
	size_t start = offset;
	offset += size;

	modelBuffer.pModel       = model;
	modelBuffer.resourceType = resourceType;
	modelBuffer.offset       = start;
	modelBuffer.len          = size;

	return modelBuffer;
}





//==============================================================================
// Renderer_UpdateModelBuffer
//------------------------------------------------------------------------------
void
Renderer_UpdateModelBuffer(
	VSR_Renderer* renderer,
	Renderer_ModelBuffer modelBuffer,
	size_t size)
{

}





//==============================================================================
// Renderer_GetModelBufferIndex
//------------------------------------------------------------------------------
size_t
Renderer_GetModelBufferIndex(
	VSR_Renderer* renderer,
	VSR_Model* model)
{
	size_t index = -1; // flag

	for(size_t i = 0; i < renderer->subStructs->modelBuffersSize; i++)
	{
		if(renderer->subStructs->modelBuffers[i].pModel == model)
		{
			index = i;
			break;
		}
	}

	return index;
}





//==============================================================================
// Renderer_AppendModelBuffer
//------------------------------------------------------------------------------
size_t
Renderer_AppendModelBuffer(
	VSR_Renderer* renderer,
	Renderer_ModelBuffer modelBuffer)
{
	renderer->subStructs->modelBuffers = realloc(
		renderer->subStructs->modelBuffers,
		sizeof(Renderer_ModelBuffer) * (renderer->subStructs->modelBuffersSize + 1));

	renderer->subStructs->modelBuffers[renderer->subStructs->modelBuffersSize++] = modelBuffer;
	return renderer->subStructs->modelBuffersSize-1;
}


void* Renderer_MapModelBuffer(
	VSR_Renderer* renderer,
	size_t index)
{
	void* mappedData;
	vkMapMemory(renderer->subStructs->logicalDevice.device,
				renderer->subStructs->deviceBufferMemory,
				renderer->subStructs->modelBuffers[index].offset,
				renderer->subStructs->modelBuffers[index].len,
				0L,
				&mappedData);

	return mappedData;
}

void Renderer_UnmapModelBuffer(
	VSR_Renderer* renderer,
	void* mappedData)
{
	vkUnmapMemory(renderer->subStructs->logicalDevice.device,
				  renderer->subStructs->deviceBufferMemory);
}

int
VSR_RenderModels(
	VSR_Renderer* renderer,
	VSR_Model* model,
	VSR_Transform* transforms,
	size_t batchCount)
{
	VkCommandBuffer* buffs =  renderer->subStructs
		->pipeline->subStructs->commandPool.commandBuffers;

	size_t i = renderer->subStructs->currentFrame;
	{
		size_t index = Renderer_GetModelBufferIndex(renderer, model);
		Renderer_ModelBuffer buf = renderer->subStructs->modelBuffers[index];

		VkDeviceSize offset = buf.offset;
		uint32_t count = model->vertexCount;

		vkCmdBindVertexBuffers(buffs[i],
							   0,
							   1,
							   &renderer->subStructs->deviceBuffer,
							   &offset);

		vkCmdDraw(buffs[i], count, 1, 0, 0);
	}

	return 0;
}
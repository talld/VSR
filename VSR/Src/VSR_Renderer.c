#include "VSR_Renderer.h"

#include <vulkan/vulkan.h>
#include <VSR_error.h>
#include <VSR_Model.h>
#include <VSR_Sampler.h>





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
// Renderer_AllocateBuffers
//------------------------------------------------------------------------------
void
Renderer_AllocateBuffers(
	VSR_Renderer* renderer)
{
	VkBufferUsageFlagBits VUVIStageBufferBits =
							  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
							  | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
							  | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VkMemoryPropertyFlagBits VUVIstagingProps =
								 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
								 | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	renderer->subStructs->VUVIStagingBuffer = Renderer_MemoryCreate(
		renderer,
		1024 * 1024 * 1024,
		VUVIStageBufferBits,
		VUVIstagingProps);

	VkBufferUsageFlagBits VUVIGPUBufferBits =
							  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
							  | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
							  | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VkMemoryPropertyFlagBits VUVIGPUProps =
								 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	renderer->subStructs->VUVIGPUBuffer = Renderer_MemoryCreate(
		renderer,
		1024 * 1024 * 1024,
		VUVIGPUBufferBits,
		VUVIGPUProps);

	VkBufferUsageFlagBits USDStageBufferBits =
							  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
							  | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
							  | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VkMemoryPropertyFlagBits USDStageProps =
								 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
								 | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;;

	renderer->subStructs->USDStagingBuffer = Renderer_MemoryCreate(
		renderer,
		1024 * 1024 * 1024,
		USDStageBufferBits,
		USDStageProps);

	VkBufferUsageFlagBits USDGPUBufferBits =
							  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
							  | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
							  | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT
							  | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT
							  | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VkMemoryPropertyFlagBits USDGPUProps =
								 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	renderer->subStructs->USDGPUBuffer = Renderer_MemoryCreate(
		renderer,
		128 * 1024 * 1024,
		USDGPUBufferBits,
		USDGPUProps);
}





//==============================================================================
// Renderer_AllocateBuffers
//------------------------------------------------------------------------------
void
Renderer_FreeBuffers(
	VSR_Renderer* renderer)
{
	Renderer_MemoryDestroy(renderer, renderer->subStructs->VUVIGPUBuffer);
	Renderer_MemoryDestroy(renderer, renderer->subStructs->VUVIStagingBuffer);
	Renderer_MemoryDestroy(renderer, renderer->subStructs->USDGPUBuffer);
	Renderer_MemoryDestroy(renderer, renderer->subStructs->USDStagingBuffer);

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

	Renderer_AllocateBuffers(renderer);

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
	Renderer_FreeBuffers(renderer);

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


static VkCommandBuffer cBuff = NULL;

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

	cBuff = GraphicsPipeline_CommandPoolAllocateGraphicsBuffer(renderer,
															   renderer->subStructs->pipeline);

	/// step 1 command record
	GraphicsPipeline_CommandBufferRecordStart(
		renderer,
		renderer->subStructs->pipeline,
		cBuff);
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
		renderer->subStructs->pipeline,
		cBuff);

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
	submitInfo.pCommandBuffers = &cBuff;

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

int
	VSR_RenderModels(
	VSR_Renderer* renderer,
	VSR_Model* model,
	VSR_Mat4* transforms,
	size_t batchCount)
{

	for(size_t i = 0; i < batchCount; i++)
	{
		vkCmdBindVertexBuffers(
			cBuff,
			0,
			1,
			&renderer->subStructs->VUVIGPUBuffer.buffer,
			&model->vertices->offset);

		if(model->mesh->UVs)
		{
			vkCmdBindVertexBuffers(
				cBuff,
				1,
				1,
				&renderer->subStructs->VUVIGPUBuffer.buffer,
				&model->UVs->offset);
		}

		Renderer_PushConstantsVertex pushConstantsVertex = (Renderer_PushConstantsVertex){0};

		if(transforms)
		{
			pushConstantsVertex.MVP = transforms[i];
		}
		else // assign identity matrix
		{
			pushConstantsVertex.MVP.m0 = 1;
			pushConstantsVertex.MVP.m5 = 1;
			pushConstantsVertex.MVP.m10 = 1;
			pushConstantsVertex.MVP.m15 = 1;
		}

		if(model->sampler)
		{
			pushConstantsVertex.imageIndex = model->sampler->index;
		}
		else
		{
			pushConstantsVertex.imageIndex = 0;
		}

		vkCmdPushConstants(
			cBuff,
			renderer->subStructs->pipeline->subStructs->graphicPipeline.pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(Renderer_PushConstantsVertex),
			&pushConstantsVertex);

		if (model->indexCount)
		{
			vkCmdBindIndexBuffer(
				cBuff,
				renderer->subStructs->VUVIGPUBuffer.buffer,
				model->indices->offset,
				VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(
				cBuff,
				model->indexCount,
				batchCount,
				0,
				0,
				0);
		}
		else
		{
			// don't know why i couldn't just do index = 0 for this...
			vkCmdDraw(
				cBuff,
				model->vertexCount,
				batchCount,
				0,
				0);
		}
	}
	return 0;
}
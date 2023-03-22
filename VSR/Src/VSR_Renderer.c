#include "VSR_Renderer.h"
#include "stdio.h"
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
	///////////////////////////////////////////
	/// permanent vertex (index UV) storage ///
	///////////////////////////////////////////
	VkBufferUsageFlagBits VIBufferBits =
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		| VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	/////////////
	/// stage ///
	/////////////
	VkBufferUsageFlagBits VIStageBufferBits =
		VIBufferBits
		| VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VkMemoryPropertyFlagBits VIStagingProps =
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	renderer->subStructs->VIStagingBuffer = Renderer_MemoryCreate(
		renderer,
		512 * 1024 * 1024,
		VIStageBufferBits,
		VIStagingProps);

	////////////
	/// gpu ///
	///////////
	VkBufferUsageFlagBits VIGPUBufferBits =
		VIBufferBits
		| VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VkMemoryPropertyFlagBits VIGPUProps =
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	renderer->subStructs->VIGPUBuffer = Renderer_MemoryCreate(
		renderer,
		448 * 1024 * 1024,
		VIGPUBufferBits,
		VIGPUProps);

	///////////////////////////////////////////////////////
	/// Scratch GPU Mem (allocs reset at end of render) ///
	///////////////////////////////////////////////////////
	VkBufferUsageFlagBits scratchGPUBits =
		VIBufferBits
		| VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VkMemoryPropertyFlagBits scratchGPUProps =
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	renderer->subStructs->scratchBuffer = Renderer_MemoryCreate(
		renderer,
		128 * 1024 * 1024,
		scratchGPUBits,
		scratchGPUProps);

	//////////////////////////////////////////////
	/// Uniform + sampler + descriptor storage ///
	//////////////////////////////////////////////
	VkBufferUsageFlagBits USDBufferBits =
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
		| VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
		| VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT
		| VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

	/////////////
	/// stage ///
	/////////////
	VkBufferUsageFlagBits USDStageBufferBits =
		USDBufferBits
		| VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VkMemoryPropertyFlagBits USDStageProps =
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;;

	renderer->subStructs->USDStagingBuffer = Renderer_MemoryCreate(
		renderer,
		512 * 1024 * 1024,
		USDStageBufferBits,
		USDStageProps);

	///////////
	/// GPU ///
	///////////
	VkBufferUsageFlagBits USDGPUBufferBits =
		USDBufferBits
		| VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VkMemoryPropertyFlagBits USDGPUProps =
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	renderer->subStructs->USDGPUBuffer = Renderer_MemoryCreate(
		renderer,
		448 * 1024 * 1024,
		USDGPUBufferBits,
		USDGPUProps);


	/////////////////////////////
	/// extra user descriptor ///
	/////////////////////////////
	for(size_t i = 0; i < renderer->subStructs->extraDescriptorCount; i++)
	{
		renderer->subStructs->extraDescriptorAllocs[i] =
			Renderer_MemoryAllocate(
				renderer,
				&renderer->subStructs->USDGPUBuffer,
				renderer->subStructs->extraDescriptorSizes[i],
				0
				);
	}
}





//==============================================================================
// Renderer_AllocateBuffers
//------------------------------------------------------------------------------
void
Renderer_FreeBuffers(
	VSR_Renderer* renderer)
{
	Renderer_MemoryDestroy(renderer, renderer->subStructs->VIGPUBuffer);
	Renderer_MemoryDestroy(renderer, renderer->subStructs->VIStagingBuffer);
	Renderer_MemoryDestroy(renderer, renderer->subStructs->USDGPUBuffer);
	Renderer_MemoryDestroy(renderer, renderer->subStructs->USDStagingBuffer);

}


VSR_RendererCreateInfo*
VSR_RendererRequestTextureCount(
	VSR_RendererCreateInfo* createInfo,
	size_t count)
{
	createInfo->texturePoolSize = count;
	return createInfo;
}

VSR_RendererCreateInfo*
VSR_RendererRequestDescriptor(
	VSR_RendererCreateInfo* createInfo,
	size_t index,
	size_t size)
{
	if(index < kMaxSupportedStorageBuffers)
	{
		createInfo->extraDescriptorSizes[index] = size;
		if(index+1 > createInfo->extraDescriptorCount)
		{
			createInfo->extraDescriptorCount = index+1;
		}
	}
	return createInfo;
}


//==============================================================================
// VSR_RendererGenerateCreateInfo
//------------------------------------------------------------------------------
VSR_RendererCreateInfo*
VSR_RendererGenerateCreateInfo(
	SDL_Window* window)
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

	//////////////////////
	/// setup defaults ///
	//////////////////////
	createInfo->texturePoolSize = 256;

	createInfo->geometryShaderRequested = SDL_FALSE;
	createInfo->tessellationShaderRequested = SDL_FALSE;


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

	// TODO: check
	renderer->subStructs->texturePoolSize = rendererCreateInfo->texturePoolSize;

	renderer->subStructs->extraDescriptorSizes = rendererCreateInfo->extraDescriptorSizes;
	renderer->subStructs->extraDescriptorCount = rendererCreateInfo->extraDescriptorCount;

	VSR_InstanceCreate(renderer, rendererCreateInfo->subStructs);
	VSR_SurfaceCreate(renderer, rendererCreateInfo->subStructs);
	VSR_PhysicalDeviceSelect(renderer, rendererCreateInfo->subStructs);
	VSR_DeviceQueuesCreate(renderer, rendererCreateInfo->subStructs);
	VSR_LogicalDeviceCreate(renderer, rendererCreateInfo->subStructs);

	Renderer_AllocateBuffers(renderer);

	// TODO: move this to its own VSR_GraphicsPipeline struct
	VSR_SwapchainCreate(renderer, rendererCreateInfo->subStructs);

	Renderer_CreateSyncObjects(renderer);

	// stage 2
	Renderer_DescriptorPoolPopulateCreateInfo(renderer, rendererCreateInfo);
	Renderer_CommandPoolPopulateCreateInfo(renderer, rendererCreateInfo);

	Renderer_DescriptorPoolCreate(renderer, rendererCreateInfo);
	Renderer_CommandPoolCreate(renderer, rendererCreateInfo);


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
	Renderer_CommandPoolDestroy(renderer);
	Renderer_DescriptorPoolDestroy(renderer);

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

	cBuff = Renderer_CommandPoolAllocateGraphicsBuffer(
		renderer);

	/// step 1 command record
	Renderer_CommandBufferRecordStart(
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
	Renderer_CommandBufferRecordEnd(
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

	vkQueueSubmit(renderer->subStructs->deviceQueues.QList[kGraphicsQueueIndex],
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

	vkQueuePresentKHR(renderer->subStructs->deviceQueues.QList[kGraphicsQueueIndex],
					  &presentInfo);

	*frameIndex = (*frameIndex + 1) % renderer->subStructs->swapchain.imageViewCount;

	Renderer_MemoryReset(&renderer->subStructs->scratchBuffer);
}

int
	VSR_RenderModels(
	VSR_Renderer* renderer,
	VSR_Model* model,
	VSR_Mat4* transforms,
	VSR_Sampler* samplers,
	size_t batchCount)
{
	//////////////////////
	/// push constants ///
	//////////////////////
	vkCmdPushConstants(
		cBuff,
		renderer->subStructs->pipeline->subStructs->graphicPipeline.pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(VSR_PushConstants),
		&renderer->subStructs->pushConstantsVertex);

	vkCmdPushConstants(
		cBuff,
		renderer->subStructs->pipeline->subStructs->graphicPipeline.pipelineLayout,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		sizeof(VSR_PushConstants),
		sizeof(VSR_PushConstants),
		&renderer->subStructs->pushConstantsFragment);

	/////////////////////////////////////////////////////////////////////
	/// move all the instanced stuff into per instance scratch memory ///
	/////////////////////////////////////////////////////////////////////
	size_t mat4ByteCount = sizeof(VSR_Mat4) * batchCount;
	size_t samplerByteCount = sizeof(uint32_t) * batchCount;

	/////////////////////
	/// load matrices ///
	/////////////////////
	Renderer_MemoryAlloc* mat4StageAlloc = Renderer_MemoryAllocate(
		renderer,
		&renderer->subStructs->VIStagingBuffer,
		mat4ByteCount,
		0
	);
	Renderer_MemoryAlloc* mat4Alloc = Renderer_MemoryAllocate(
		renderer,
		&renderer->subStructs->scratchBuffer,
		mat4ByteCount,
		0
	);
	VSR_Mat4* p = Renderer_MemoryAllocMap(renderer, mat4StageAlloc);

	for(size_t i = 0; i < batchCount; i++)
	{
		VSR_Mat4 rotate = transforms[i];
		p[i] = rotate;
	}
	Renderer_MemoryAllocUnmap(renderer, mat4StageAlloc);

	Renderer_MemoryTransferAlloc(renderer, mat4Alloc, mat4StageAlloc);
	Renderer_MemoryAllocFree(renderer, mat4StageAlloc);

	/////////////////////
	/// load samplers ///
	/////////////////////
	Renderer_MemoryAlloc* samplerStageAlloc = Renderer_MemoryAllocate(
		renderer,
		&renderer->subStructs->VIStagingBuffer,
		samplerByteCount,
		0
	);
	Renderer_MemoryAlloc* samplerAlloc = Renderer_MemoryAllocate(
		renderer,
		&renderer->subStructs->scratchBuffer,
		samplerByteCount,
		0
	);
	uint32_t* ip = Renderer_MemoryAllocMap(renderer, samplerStageAlloc);
	for(size_t i = 0; i < batchCount; i++)
	{
		ip[i] = (int32_t)samplers[i].index;
	}
	Renderer_MemoryAllocUnmap(renderer, samplerStageAlloc);
	Renderer_MemoryTransferAlloc(renderer, samplerAlloc, samplerStageAlloc);
	Renderer_MemoryAllocFree(renderer, samplerStageAlloc);

	////////////////////////
	/// per vertex stuff ///
	////////////////////////
	enum {kPerVertexBufferCount = 3};
	VkBuffer perVertexBuffers[kPerVertexBufferCount] = {
		model->vertices->src->buffer,
		model->normals->src->buffer,
		model->UVs->src->buffer,
	};

	VkDeviceSize perVertexBufferOffsets[kPerVertexBufferCount] = {
		model->vertices->offset,
		model->normals->offset,
		model->UVs->offset
	};

	vkCmdBindVertexBuffers(
		cBuff,
		0,
		kPerVertexBufferCount,
		perVertexBuffers,
		perVertexBufferOffsets);

	//////////////////////////
	/// per instance stuff ///
	//////////////////////////
	enum {kPerInstanceBufferCount = 2};
	VkBuffer perInstanceBuffers[kPerInstanceBufferCount] = {
		samplerAlloc->src->buffer,
		mat4Alloc->src->buffer,

	};

	VkDeviceSize perInstanceBufferOffsets[kPerInstanceBufferCount] = {
		samplerAlloc->offset,
		mat4Alloc->offset,
	};

	vkCmdBindVertexBuffers(
		cBuff,
		kPerVertexBufferCount, // start were per vertex data ended
		kPerInstanceBufferCount,
		perInstanceBuffers,
		perInstanceBufferOffsets);

	enum {kDescriptorSetCount = 2};
	VkDescriptorSet descriptorSets[kDescriptorSetCount] =
		{
			renderer->subStructs->descriptorPool.globalSet,
			renderer->subStructs->descriptorPool.userSet
		};

	vkCmdBindDescriptorSets(
		cBuff,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		renderer->subStructs->pipeline->subStructs->graphicPipeline.pipelineLayout,
		0,
		1 + (renderer->subStructs->extraDescriptorCount > 0),
		descriptorSets,
		0,
		NULL
		);

	if (model->indexCount)
	{
		vkCmdBindIndexBuffer(
			cBuff,
			model->indices->src->buffer,
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
	return 0;
}

void
VSR_RendererSetVertexConstants(
	VSR_Renderer* renderer,
	VSR_PushConstants const* pushConstants)
{
	renderer->subStructs->pushConstantsVertex = *pushConstants;
}

void
VSR_RendererSetFragmentConstants(
	VSR_Renderer* renderer,
	VSR_PushConstants const* pushConstants)
{
	renderer->subStructs->pushConstantsFragment = *pushConstants;
}

void
VSR_RendererWriteDescriptor(
	VSR_Renderer* renderer,
	size_t index,
	size_t offset,
	void* data,
	size_t len)
{
	Renderer_MemoryAlloc* stageAlloc = Renderer_MemoryAllocate(
		renderer,
		&renderer->subStructs->USDStagingBuffer,
		len,
		0);

	void* p = Renderer_MemoryAllocMap(renderer, stageAlloc);
	memcpy(p, data, len);
	Renderer_MemoryAllocUnmap(renderer, stageAlloc);

	Renderer_MemoryTransferAlloc(
		renderer,
		renderer->subStructs->extraDescriptorAllocs[index],
		stageAlloc
	);

	Renderer_MemoryAllocFree(renderer, stageAlloc);

	VkDescriptorBufferInfo bufferInfo = (VkDescriptorBufferInfo){0};
	bufferInfo.offset = renderer->subStructs->extraDescriptorAllocs[index]->offset;
	bufferInfo.buffer = renderer->subStructs->extraDescriptorAllocs[index]->src->buffer;
	bufferInfo.range = renderer->subStructs->extraDescriptorAllocs[index]->size;

	VkWriteDescriptorSet bufferWrite = (VkWriteDescriptorSet){0};
	bufferWrite.pNext = NULL;
	bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bufferWrite.dstSet = renderer->subStructs->descriptorPool.userSet;
	bufferWrite.dstBinding = index;
	bufferWrite.dstArrayElement =0;
	bufferWrite.descriptorCount = 1;
	bufferWrite.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(renderer->subStructs->logicalDevice.device,
	                       1, &bufferWrite,
	                       0, NULL);
	
}

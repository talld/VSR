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

	listSize = renderer->swapchain.imageViewCount * sizeof(VkSemaphore);
	renderer->imageCanBeRead = SDL_malloc(listSize);
	renderer->imageCanBeWritten = SDL_malloc(listSize);

	listSize = renderer->swapchain.imageViewCount * sizeof(VkFence);
	renderer->imageFinished = SDL_malloc(listSize);

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

	for(size_t i = 0; i < renderer->swapchain.imageViewCount; i++)
	{
		vkCreateSemaphore(renderer->logicalDevice.device,
						  &semaphoreInfo,
						  VSR_GetAllocator(),
						  &renderer->imageCanBeRead[i]);

		vkCreateSemaphore(renderer->logicalDevice.device,
						  &semaphoreInfo,
						  VSR_GetAllocator(),
						  &renderer->imageCanBeWritten[i]);

		vkCreateFence(renderer->logicalDevice.device,
					  &fenceInfo,
					  VSR_GetAllocator(),
					  &renderer->imageFinished[i]);
	}

}





//==============================================================================
// Renderer_DestroySyncObjects
//------------------------------------------------------------------------------
void Renderer_DestroySyncObjects(VSR_Renderer* renderer)
{
	for(size_t i = 0; i < renderer->swapchain.imageViewCount; i++)
	{
		vkDestroySemaphore(renderer->logicalDevice.device,
						   renderer->imageCanBeWritten[i],
						   VSR_GetAllocator());

		vkDestroySemaphore(renderer->logicalDevice.device,
						   renderer->imageCanBeRead[i],
						   VSR_GetAllocator());

		vkDestroyFence(renderer->logicalDevice.device,
					   renderer->imageFinished[i],
					   VSR_GetAllocator());
	}

	SDL_free(renderer->imageCanBeWritten);
	SDL_free(renderer->imageCanBeRead);
	SDL_free(renderer->imageFinished);

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

	renderer->VIStagingBuffer = Renderer_MemoryCreate(
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

	renderer->VIGPUBuffer = Renderer_MemoryCreate(
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

	renderer->scratchBuffer = Renderer_MemoryCreate(
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

	renderer->USDStagingBuffer = Renderer_MemoryCreate(
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

	renderer->USDGPUBuffer = Renderer_MemoryCreate(
		renderer,
		448 * 1024 * 1024,
		USDGPUBufferBits,
		USDGPUProps);


	/////////////////////////////
	/// extra user descriptor ///
	/////////////////////////////
	for(size_t i = 0; i < renderer->extraDescriptorCount; i++)
	{
		renderer->extraDescriptorAllocs[i] =
			Renderer_MemoryAllocate(
				renderer,
				&renderer->USDGPUBuffer,
				renderer->extraDescriptorSizes[i],
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
	Renderer_MemoryDestroy(renderer, renderer->VIGPUBuffer);
	Renderer_MemoryDestroy(renderer, renderer->VIStagingBuffer);
	Renderer_MemoryDestroy(renderer, renderer->USDGPUBuffer);
	Renderer_MemoryDestroy(renderer, renderer->USDStagingBuffer);

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
	VSR_InstancePopulateCreateInfo(createInfo);
	VSR_DeviceQueuesPopulateCreateInfo(createInfo);
    VSR_LogicalDevicePopulateCreateInfo(createInfo);
	VSR_SwapchainPopulateCreateInfo(createInfo);

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
	
	//////////////////////////////////
	/// pass info to new structure ///
	//////////////////////////////////
	renderer->SDLWindow = rendererCreateInfo->SDLWindow;

	// TODO: check
	renderer->texturePoolSize = rendererCreateInfo->texturePoolSize;

	renderer->extraDescriptorSizes = rendererCreateInfo->extraDescriptorSizes;
	renderer->extraDescriptorCount = rendererCreateInfo->extraDescriptorCount;

	renderer->pushConstantsVertex = (VSR_PushConstants){0};
	renderer->pushConstantsVertex.Projection.m0 = -1.81066f;
	renderer->pushConstantsVertex.Projection.m5 = 2.41421342f;
	renderer->pushConstantsVertex.Projection.m10 = -1.002002f;
	renderer->pushConstantsVertex.Projection.m11 = -1.f;
	renderer->pushConstantsVertex.Projection.m14 = 4.f;
	renderer->pushConstantsVertex.Projection.m15 = 4.f;

	VSR_InstanceCreate(renderer, rendererCreateInfo);
	VSR_SurfaceCreate(renderer, rendererCreateInfo);
	VSR_PhysicalDeviceSelect(renderer, rendererCreateInfo);
	VSR_DeviceQueuesCreate(renderer, rendererCreateInfo);
	VSR_LogicalDeviceCreate(renderer, rendererCreateInfo);

	Renderer_AllocateBuffers(renderer);

	// TODO: move this to its own VSR_GraphicsPipeline struct
	VSR_SwapchainCreate(renderer, rendererCreateInfo);

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
	vkDeviceWaitIdle(renderer->logicalDevice.device);

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

	/////////////////////
	/// Free renderer ///
	/////////////////////
	SDL_free((void*)renderer);
}

void VSR_RendererSetPipeline(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline)
{
	renderer->pipeline = pipeline;
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
	if(!renderer->pipeline) return;

	///////////////
	/// aliases ///
	///////////////
	size_t* frameIndex = &renderer->currentFrame;

	////////////////////////////////////////////////////////////////
	/// wait for requested image to be done ( ready to use again ///
	////////////////////////////////////////////////////////////////
	vkWaitForFences(renderer->logicalDevice.device,
					1,
					&renderer->imageFinished[*frameIndex],
					VK_TRUE,
					-1);

	//////////////////
	/// reset sync ///
	//////////////////
	vkResetFences(renderer->logicalDevice.device,
				  1,
				  &renderer->imageFinished[*frameIndex]);

	///////////////////////////
	/// get swapchain image ///
	///////////////////////////
	vkAcquireNextImageKHR(renderer->logicalDevice.device,
						  renderer->swapchain.swapchain,
						  -1,
						  renderer->imageCanBeWritten[*frameIndex],
						  VK_NULL_HANDLE,
						  &renderer->imageIndex);

	cBuff = Renderer_CommandPoolAllocateGraphicsBuffer(
		renderer);

	/// step 1 command record
	Renderer_CommandBufferRecordStart(
		renderer,
		renderer->pipeline,
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
	if(!renderer->pipeline) return;


	///////////////
	/// aliases ///
	///////////////
	size_t* frameIndex = &renderer->currentFrame;


	/// step2 command record
	Renderer_CommandBufferRecordEnd(
		renderer,
		renderer->pipeline,
		cBuff);

	////////////////////////////////
	/// submit commands to queue ///
	////////////////////////////////
	VkPipelineStageFlags waitStages[1] =
							 {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	VkSubmitInfo submitInfo = (VkSubmitInfo){0};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &renderer->imageCanBeWritten[*frameIndex];
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderer->imageCanBeRead[*frameIndex];
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cBuff;

	vkQueueSubmit(renderer->deviceQueues.QList[kGraphicsQueueIndex],
				  1,
				  &submitInfo,
				  renderer->imageFinished[*frameIndex]);

	///////////////////////////////////////////
	/// present queue information to screen ///
	///////////////////////////////////////////
	VkPresentInfoKHR presentInfo = (VkPresentInfoKHR){0};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderer->imageCanBeRead[*frameIndex];
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &renderer->swapchain.swapchain;
	presentInfo.pImageIndices = &renderer->imageIndex;

	vkQueuePresentKHR(renderer->deviceQueues.QList[kGraphicsQueueIndex],
					  &presentInfo);

	*frameIndex = (*frameIndex + 1) % renderer->swapchain.imageViewCount;

	Renderer_MemoryReset(&renderer->scratchBuffer);
}

int
	VSR_RenderModels(
	VSR_Renderer* renderer,
	VSR_Model* model,
	VSR_Mat4* transforms,
	VSR_Sampler** samplers,
	size_t batchCount)
{
	//////////////////////
	/// push constants ///
	//////////////////////
	vkCmdPushConstants(
		cBuff,
		renderer->pipeline->graphicPipeline.pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(VSR_PushConstants),
		&renderer->pushConstantsVertex);

	vkCmdPushConstants(
		cBuff,
		renderer->pipeline->graphicPipeline.pipelineLayout,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		sizeof(VSR_PushConstants),
		sizeof(VSR_PushConstants),
		&renderer->pushConstantsFragment);

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
		&renderer->VIStagingBuffer,
		mat4ByteCount,
		0
	);
	Renderer_MemoryAlloc* mat4Alloc = Renderer_MemoryAllocate(
		renderer,
		&renderer->scratchBuffer,
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
		&renderer->VIStagingBuffer,
		samplerByteCount,
		0
	);
	Renderer_MemoryAlloc* samplerAlloc = Renderer_MemoryAllocate(
		renderer,
		&renderer->scratchBuffer,
		samplerByteCount,
		0
	);
	uint32_t* ip = Renderer_MemoryAllocMap(renderer, samplerStageAlloc);
	for(size_t i = 0; i < batchCount; i++)
	{
		ip[i] = (int32_t)samplers[i]->index;
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
			renderer->descriptorPool.globalSet,
			renderer->descriptorPool.userSet
		};

	vkCmdBindDescriptorSets(
		cBuff,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		renderer->pipeline->graphicPipeline.pipelineLayout,
		0,
		1 + (renderer->extraDescriptorCount > 0),
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
	renderer->pushConstantsVertex = *pushConstants;
}

void
VSR_RendererSetFragmentConstants(
	VSR_Renderer* renderer,
	VSR_PushConstants const* pushConstants)
{
	renderer->pushConstantsFragment = *pushConstants;
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
		&renderer->USDStagingBuffer,
		len,
		0);

	void* p = Renderer_MemoryAllocMap(renderer, stageAlloc);
	memcpy(p, data, len);
	Renderer_MemoryAllocUnmap(renderer, stageAlloc);

	Renderer_MemoryTransferAlloc(
		renderer,
		renderer->extraDescriptorAllocs[index],
		stageAlloc
	);

	Renderer_MemoryAllocFree(renderer, stageAlloc);

	VkDescriptorBufferInfo bufferInfo = (VkDescriptorBufferInfo){0};
	bufferInfo.offset = renderer->extraDescriptorAllocs[index]->offset;
	bufferInfo.buffer = renderer->extraDescriptorAllocs[index]->src->buffer;
	bufferInfo.range = renderer->extraDescriptorAllocs[index]->size;

	VkWriteDescriptorSet bufferWrite = (VkWriteDescriptorSet){0};
	bufferWrite.pNext = NULL;
	bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bufferWrite.dstSet = renderer->descriptorPool.userSet;
	bufferWrite.dstBinding = index;
	bufferWrite.dstArrayElement =0;
	bufferWrite.descriptorCount = 1;
	bufferWrite.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(renderer->logicalDevice.device,
	                       1, &bufferWrite,
	                       0, NULL);
	
}

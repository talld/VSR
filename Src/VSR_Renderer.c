#include "VSR_Renderer.h"

#include "VSR_Mat4.h"
#include "VSR_Sampler.h"

#include "fallbackTexture.h"
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

	listSize = renderer->swapchain.imageViewCount * sizeof(VkSemaphore);
	renderer->imageCanBeRead = SDL_malloc(listSize);
	renderer->imageCanBeWritten = SDL_malloc(listSize);

	listSize = renderer->swapchain.imageViewCount * sizeof(VSR_GenerationalFence);
	renderer->imageFinished = SDL_calloc(1, listSize);

	listSize = renderer->swapchain.imageViewCount * sizeof(size_t);
	renderer->generationAcquired = SDL_malloc(listSize);
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
		                  &renderer->imageCanBeRead[i]
		);

		vkCreateSemaphore(renderer->logicalDevice.device,
		                  &semaphoreInfo,
		                  VSR_GetAllocator(),
		                  &renderer->imageCanBeWritten[i]
		);
	}

}





//==============================================================================
// Renderer_DestroySyncObjects
//------------------------------------------------------------------------------
void Renderer_DestroySyncObjects(VSR_Renderer* renderer)
{
	for(size_t i = 0; i < renderer->swapchain.imageViewCount; i++)
	{
		vkDestroySemaphore(
			renderer->logicalDevice.device,
			renderer->imageCanBeWritten[i],
			VSR_GetAllocator()
		);

		vkDestroySemaphore(
			renderer->logicalDevice.device,
			renderer->imageCanBeRead[i],
			VSR_GetAllocator()
		);
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
	VSR_Renderer* renderer,
	VSR_RendererCreateInfo* createInfo)
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

	renderer->vertexStagingBuffer = Renderer_MemoryCreate(
		renderer,
		createInfo->vertexStagingBufferSize,
		VIStageBufferBits,
		VIStagingProps
	);

	////////////
	/// gpu ///
	///////////
	VkBufferUsageFlagBits VIGPUBufferBits =
		VIBufferBits
		| VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VkMemoryPropertyFlagBits VIGPUProps =
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	renderer->perModelVertexGPUBuffer = Renderer_MemoryCreate(
		renderer,
		createInfo->perModelVertexGPUBufferSize,
		VIGPUBufferBits,
		VIGPUProps
	);

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
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	renderer->USDStagingBuffer = Renderer_MemoryCreate(
		renderer,
		createInfo->DescriptorSamplerStagingBufferSize,
		USDStageBufferBits,
		USDStageProps
	);

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
		createInfo->DescriptorSamplerGPUBufferSize,
		USDGPUBufferBits,
		USDGPUProps
	);


	/////////////////////////////
	/// extra user descriptor ///
	/////////////////////////////
	for(size_t i = 0; i < renderer->extraDescriptorCount; i++)
	{
		renderer->extraDescriptorAllocs[i] = Renderer_MemoryAllocate(
			renderer,
			renderer->USDGPUBuffer,
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
	Renderer_MemoryDestroy(renderer, renderer->perModelVertexGPUBuffer);
	Renderer_MemoryDestroy(renderer, renderer->vertexStagingBuffer);
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
	createInfo->cmdBuffersPerPool = 15;

	createInfo->geometryShaderRequested = SDL_FALSE;
	createInfo->tessellationShaderRequested = SDL_FALSE;

	createInfo->vertexStagingBufferSize = 128 * 1024 * 1024;
	createInfo->perModelVertexGPUBufferSize = 256 * 1024 * 1024;
	createInfo->DescriptorSamplerStagingBufferSize = 128 * 1024 * 1024;
	createInfo->DescriptorSamplerGPUBufferSize = 256 * 1024 * 1024;

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

	// stage one object creation
	VSR_InstanceCreate(renderer, rendererCreateInfo);
	VSR_SurfaceCreate(renderer, rendererCreateInfo);
	VSR_PhysicalDeviceSelect(renderer, rendererCreateInfo);
	VSR_DeviceQueuesCreate(renderer, rendererCreateInfo);
	VSR_LogicalDeviceCreate(renderer, rendererCreateInfo);
	VSR_SwapchainCreate(renderer, rendererCreateInfo);

	Renderer_DescriptorPoolPopulateCreateInfo(renderer, rendererCreateInfo);
	Renderer_CommandPoolPopulateCreateInfo(renderer, rendererCreateInfo);

	Renderer_DescriptorPoolCreate(renderer, rendererCreateInfo);
	Renderer_CommandPoolCreate(renderer, rendererCreateInfo);

	// stage 2 mem alloc
	Renderer_AllocateBuffers(renderer, rendererCreateInfo);
	Renderer_CreateSyncObjects(renderer);

	// stage 3 memory allocations

	SDL_Surface* sur = SDL_CreateRGBSurfaceWithFormat(
		0,
		kFallBackTextureWidth,
		kFallBackTextureHeight,
		kFallBackTextureDepth,
		kFallBackTextureFormat);
	sur->pixels = kFallBackTexturePixels;
	VSR_CreateTextureSampler(renderer);
	renderer->defaultSampler = VSR_SamplerCreate(
		renderer,
		0,
		sur,
		0
	);

	for(size_t i = 0; i < renderer->texturePoolSize; i++)
	{
		VSR_SamplerWriteToDescriptor(renderer, i, renderer->defaultSampler );
	}

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

	VSR_SamplerFree(renderer, renderer->defaultSampler);
	vkDestroySampler(
		renderer->logicalDevice.device,
		VSR_GetTextureSampler(renderer),
		VSR_GetAllocator()
	);

	////////////////////////////////////////
	/// Destroy VkStructs Vulkan objects ///
	////////////////////////////////////////

	Renderer_FreeBuffers(renderer);

	VSR_SwapchainDestroy(renderer);
	Renderer_CommandPoolDestroy(renderer);
	Renderer_DescriptorPoolDestroy(renderer);

	Renderer_DestroySyncObjects(renderer);
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

//==============================================================================
// VSR_RendererBeginPass
//------------------------------------------------------------------------------
QueuedRenderCommand*
VSR_RendererNewQueuedRenderCommand(
	VSR_Renderer* renderer)
{
	VkCommandBuffer cBuff = Renderer_CommandPoolAllocateGraphicsBuffer(
		renderer,
		NULL
		);

	VkCommandBufferInheritanceInfo inheritanceInfo = (VkCommandBufferInheritanceInfo){0};

	size_t index = 0;
	if(!renderer->pipeline->renderTarget)
	{
		index = renderer->imageIndex;
	}

	inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritanceInfo.framebuffer = renderer->pipeline->framebuffers[index]->frame;
	inheritanceInfo.renderPass = renderer->pipeline->renderPass->renderPass;
	inheritanceInfo.subpass = 0;
	inheritanceInfo.occlusionQueryEnable = VK_FALSE;
	inheritanceInfo.queryFlags = 0;
	inheritanceInfo.pipelineStatistics = 0;

	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = NULL;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = &inheritanceInfo;

	vkBeginCommandBuffer(cBuff, &beginInfo);

	VkSemaphore semaphore;
	VkSemaphoreCreateInfo semaphoreInfo = (VkSemaphoreCreateInfo){0};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.pNext = NULL;
	semaphoreInfo.flags = 0L;

	vkCreateSemaphore(renderer->logicalDevice.device,
	                  &semaphoreInfo,
	                  VSR_GetAllocator(),
	                  semaphore
	);

	QueuedRenderCommand* renderCommand = SDL_malloc(sizeof(*renderCommand));

	renderCommand->cBuff = cBuff;
	renderCommand->signalSemaphore = semaphore;

	return renderCommand;
}



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
	size_t frameIndex = renderer->currentFrame;


	////////////////////////
	/// Setup next image ///
	////////////////////////
	if(renderer->pipeline->renderTarget == NULL)
	{ /// get swapchain image ///
		vkAcquireNextImageKHR(
			renderer->logicalDevice.device,
			renderer->swapchain.swapchain,
			-1,
			renderer->imageCanBeWritten[frameIndex],
			VK_NULL_HANDLE,
			&renderer->imageIndex
		);
	}
	else
	{
		// take target out of texture index
		VSR_SamplerWriteToDescriptor(
			renderer,
			renderer->pipeline->renderTarget->textureIndex,
			renderer->defaultSampler
		);

		// prep it for being a render images
		VSR_ImageTransition(
			renderer,
			renderer->pipeline->renderTarget->image,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		);
	}

	// wait for the image to be ready to write
	if(renderer->imageFinished[frameIndex].fence != NULL
	   && renderer->generationAcquired[frameIndex] != *renderer->imageFinished[frameIndex].generation)
	{
		vkWaitForFences(
			renderer->logicalDevice.device,
			1,
			&renderer->imageFinished[frameIndex].fence,
			VK_TRUE,
			-1
		);
	}
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
	size_t frameIndex = renderer->currentFrame;


	/// step2 command record
	VSR_RendererFlushQueuedModels(renderer);

	/// step 2 command record
	VkCommandBuffer cBuff = Renderer_CommandPoolAllocateGraphicsBuffer(
		renderer,
		&renderer->imageFinished[frameIndex]
	);
	renderer->generationAcquired[frameIndex] = *renderer->imageFinished[frameIndex].generation;

	Renderer_CommandBufferRecordStart(
		renderer,
		renderer->pipeline,
		cBuff
	);



	Renderer_CommandBufferRecordEnd(
		renderer,
		renderer->pipeline,
		cBuff
	);

	////////////////////////////////
	/// submit commands to queue ///
	////////////////////////////////
	VkPipelineStageFlags waitStages[1] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};

	VkSubmitInfo submitInfo = (VkSubmitInfo){0};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cBuff;

	if(renderer->pipeline->renderTarget == NULL)
	{
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &renderer->imageCanBeWritten[frameIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderer->imageCanBeRead[frameIndex];
	}
	else
	{
		submitInfo.pWaitSemaphores = NULL;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = NULL;
		submitInfo.waitSemaphoreCount = 0;

	}

	vkQueueSubmit(
		renderer->deviceQueues.QList[kGraphicsQueueIndex],
		1,
		&submitInfo,
		renderer->imageFinished[frameIndex].fence
	);

	/////////////////////
	/// submit images ///
	/////////////////////
	if(renderer->pipeline->renderTarget == NULL)
	{
		///////////////////////////////////////////
		/// present queue information to screen ///
		///////////////////////////////////////////
		VkPresentInfoKHR presentInfo = (VkPresentInfoKHR) {0};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = NULL;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderer->imageCanBeRead[frameIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &renderer->swapchain.swapchain;
		presentInfo.pImageIndices = &renderer->imageIndex;
		presentInfo.pResults = NULL;

		vkQueuePresentKHR(
			renderer->deviceQueues.QList[kGraphicsQueueIndex],
			&presentInfo
		);
	}
	else
	{
		VSR_ImageTransition(
			renderer,
			renderer->pipeline->renderTarget->image,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		);

		// put sampler back in the texture array
		VSR_SamplerWriteToDescriptor(
			renderer,
			renderer->pipeline->renderTarget->textureIndex,
			renderer->pipeline->renderTarget
		);
	}

	renderer->currentFrame = (frameIndex + 1) % renderer->swapchain.imageViewCount;
}






void VSR_RendererPrepCommand(
	VSR_Renderer* renderer,
	QueuedRenderCommand* renderCommand)
{
	VkCommandBuffer cBuff = VSR_RendererNewQueuedRenderCommand(renderer)->cBuff;

	//////////////////////
	/// push constants ///
	//////////////////////
	uint8_t aPushVals[128];
	size_t mat4Size = sizeof(float[16]);

	if(renderer->pipeline->pushConstants.Projection)
	{
		SDL_memcpy(&aPushVals[0], &renderer->pipeline->pushConstants.Projection[0]->m0, mat4Size);


		if(renderer->pipeline->pushConstants.bytes)
		{
			SDL_memcpy(&aPushVals[64], renderer->pipeline->pushConstants.bytes, 64);
		}
		else
		{
			SDL_memcpy(&aPushVals[64], &renderer->pipeline->pushConstants.Projection[1]->m0, sizeof(float[16]));
		}
	}
	else if(renderer->pipeline->pushConstants.bytes)
	{
		SDL_memcpy(aPushVals, renderer->pipeline->pushConstants.bytes, 128);
	}

	vkCmdPushConstants(
		cBuff,
		renderer->pipeline->graphicPipeline.pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		0,
		128,
		aPushVals
	);

	///////////////////////
	/// descriptor sets ///
	///////////////////////
	enum
	{
		kDescriptorSetCount = 2
	};

	VkDescriptorSet descriptorSets[kDescriptorSetCount] = {
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

	/*
	if(renderer->queuedRenderCommandsRoot == NULL)
	{
		renderer->queuedRenderCommandsRoot = renderCommand;
	}
	else
	{
		renderer->queuedRenderCommandsFront->next = renderCommand;
	}
	renderer->queuedRenderCommandsFront = renderCommand;
	*/
}

int VSR_RendererFlushQueuedModels(VSR_Renderer* renderer)
{
	if (!renderer->activeRenderCommands) { goto SUCCESS; }

	VSR_Model *model;
	VkCommandBuffer cBuff;
	Renderer_MemoryAlloc *instanceData;
	size_t instanceCount;

	////////////////////////
	/// per vertex stuff ///
	////////////////////////
	enum { kVertexBufferCount = 4 };
	VkBuffer perVertexBuffers[kVertexBufferCount] = {
		model->vertices->src->buffer,
		model->normals->src->buffer,
		model->normals->src->buffer,
		instanceData->src->buffer,
	};

	VkDeviceSize perVertexBufferOffsets[kVertexBufferCount] = {
		model->vertices->offset,
		model->normals->offset,
		model->UVs->offset,
		instanceData->offset
	};

	vkCmdBindVertexBuffers(
		cBuff,
		0,
		kVertexBufferCount,
		perVertexBuffers,
		perVertexBufferOffsets
	);


	vkCmdBindIndexBuffer(
		cBuff,
		model->indices->src->buffer,
		model->indices->offset,
		VK_INDEX_TYPE_UINT32
	);

	vkCmdDrawIndexed(
		cBuff,
		model->indexCount,
		instanceCount,
		0, // firstIndex
		0, // vertexOffset
		0 // firstInstance
	);

	SUCCESS:
	return SDL_TRUE;
}

//==============================================================================
// VSR_RenderModels
//------------------------------------------------------------------------------
int
VSR_RenderModels(
	VSR_Renderer* renderer,
	VSR_Model* model,
	VSR_Mat4** transforms,
	VSR_Sampler** samplers,
	size_t batchCount)
{


	return 0;
}





//==============================================================================
// VSR_RendererWriteDescriptor
//------------------------------------------------------------------------------
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
		renderer->USDStagingBuffer,
		len,
		0
	);

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
	bufferInfo.offset = renderer->extraDescriptorAllocs[index]->offset + offset;
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
	                       0, NULL
	);
}

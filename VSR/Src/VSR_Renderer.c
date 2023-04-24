#include "VSR_Renderer.h"

#include "VSR_Mat4.h"

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
	renderer->imageFinished = SDL_malloc(listSize);

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

	renderer->perInstanceVertexGPUBuffer = Renderer_MemoryCreate(
		renderer,
		createInfo->perInstanceVertexGPUBufferSize,
		VIGPUBufferBits,
		VIGPUProps
	);

	size_t Mat4Size = sizeof(float[16]);
	size_t samplerSize = sizeof(uint32_t);

	size_t combinedSize = samplerSize + Mat4Size;
	size_t combinedCount = createInfo->perInstanceVertexGPUBufferSize / combinedSize;
	renderer->samplerMatrixArrayLength = combinedCount * 2;
	renderer->matrixStartIndex = combinedCount;

	size_t arraySizeBytes = renderer->samplerMatrixArrayLength * sizeof(uint64_t);
	renderer->modelSamplerMatrixArray = SDL_malloc(arraySizeBytes);
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
	Renderer_MemoryDestroy(renderer, renderer->perInstanceVertexGPUBuffer);
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
// GraphicsPipelineAllocateDepthAttachment
//------------------------------------------------------------------------------
void
Renderer_AllocateAttachments(
	VSR_Renderer* renderer)
{
	SDL_Surface* depthSur = SDL_CreateRGBSurfaceWithFormat(
		0,
		renderer->surface.surfaceWidth,
		renderer->surface.surfaceHeight,
		24,
		SDL_PIXELFORMAT_BGR888
	);
	depthSur->pixels = NULL;

	renderer->depthImage = VSR_ImageCreate(
		renderer,
		depthSur,
		VK_FORMAT_D32_SFLOAT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
	);

	renderer->depthView = VSR_ImageViewCreate(
		renderer,
		renderer->depthImage->image,
		renderer->depthImage->format,
		VK_IMAGE_ASPECT_DEPTH_BIT
	);

	SDL_FreeSurface(depthSur);
}

//==============================================================================
// GraphicsPipelineAllocateDepthAttachment
//------------------------------------------------------------------------------
void
Renderer_FreeDepthAttachment(
	VSR_Renderer* renderer)
{
	VSR_ImageViewDestroy(renderer, renderer->depthView);
	VSR_ImageDestroy(renderer, renderer->depthImage);

}

VSR_Framebuffer**
Renderer_GetSwapchainFrames(
	VSR_Renderer* renderer)
{
	static VSR_Framebuffer** swapchainFrames;

	if(!swapchainFrames)
	{
		swapchainFrames = SDL_malloc(
			renderer->swapchainImageCount * sizeof(VSR_Framebuffer)
		);

		for(size_t i = 0; i < renderer->swapchainImageCount; i++)
		{
			swapchainFrames[i] = VSR_FramebufferCreate(
				renderer,
				renderer->swapchain.pImageViews[i]
			);
		}
	}

	return swapchainFrames;
}

void
renderer_DestroySwapchainFrames(
	VSR_Renderer* renderer)
{
	VSR_Framebuffer** swapchainFrames = Renderer_GetSwapchainFrames(
		renderer
	);

	for(size_t i = 0; i < renderer->swapchainImageCount; i++)
	{
		VSR_FramebufferDestroy(renderer, swapchainFrames[i]);
	}
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
	createInfo->perModelVertexGPUBufferSize = 128 * 1024 * 1024;
	createInfo->perInstanceVertexGPUBufferSize = 128 * 1024 * 1024;

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
	Renderer_AllocateAttachments(renderer);
	Renderer_RenderPassCreate(renderer, rendererCreateInfo);

	renderer->swapchainFrames = SDL_malloc(
		renderer->swapchainImageCount * sizeof(VSR_Framebuffer)
	);

	for(size_t i = 0; i < renderer->swapchainImageCount; i++)
	{
		renderer->swapchainFrames[i] = VSR_FramebufferCreate(
			renderer,
			renderer->swapchain.pImageViews[i]);
	}

	// stage 4 set more defaults
	renderer->pushConstantsVertex = (VSR_PushConstants){0};
	/* TODO: replace with static matrix that pointer defaults to
	renderer->pushConstantsVertex.Projection->m0 = -1.81066f;
	renderer->pushConstantsVertex.Projection->m5 = 2.41421342f;
	renderer->pushConstantsVertex.Projection->m10 = -1.002002f;
	renderer->pushConstantsVertex.Projection->m11 = -1.f;
	renderer->pushConstantsVertex.Projection->m14 = 4.f;
	renderer->pushConstantsVertex.Projection->m15 = 4.f;
	*/

	SDL_Surface* sur = SDL_CreateRGBSurfaceWithFormat(
		0,
		kFallBackTextureWidth,
		kFallBackTextureHeight,
		kFallBackTextureDepth,
		kFallBackTextureFormat);
	sur->pixels = kFallBackTexturePixels;

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

	/// step 2 command record
	renderer->modelInstanceCount = 0;
	cBuff = Renderer_CommandPoolAllocateGraphicsBuffer(
		renderer,
		&renderer->imageFinished[frameIndex]
	);
	renderer->generationAcquired[frameIndex] = *renderer->imageFinished[frameIndex].generation;

	Renderer_CommandBufferRecordStart(
		renderer,
		renderer->pipeline,
		cBuff
	);
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
	Renderer_FlushQueuedModels(renderer);

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

	renderer->currentFrame = (frameIndex + 1) % renderer->swapchain.imageViewCount;
}

typedef struct QueuedRender QueuedRender;
struct QueuedRender
{
	QueuedRender* next;

	size_t instanceOffset;
	size_t instanceCount;
	VSR_Model* model;
};

static QueuedRender* sQueuedModelList;

int Renderer_FlushQueuedModels(VSR_Renderer* renderer)
{
	if(!sQueuedModelList) {goto SUCCESS;}

	//////////////////////
	/// push constants ///
	//////////////////////
	uint8_t pPushVals[256];
	SDL_memcpy(pPushVals, renderer->pushConstantsVertex.Projection, sizeof(float[16]));


	vkCmdPushConstants(
		cBuff,
		renderer->pipeline->graphicPipeline.pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		0,
		128,
		pPushVals
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

	while (sQueuedModelList != NULL)
	{

		////////////////////////
		/// per vertex stuff ///
		////////////////////////
		enum {kPerVertexBufferCount = 3};
		VkBuffer perVertexBuffers[kPerVertexBufferCount] = {
			renderer->perModelVertexGPUBuffer->buffer,
			renderer->perModelVertexGPUBuffer->buffer,
			renderer->perModelVertexGPUBuffer->buffer,
		};

		VkDeviceSize perVertexBufferOffsets[kPerVertexBufferCount] = {
			sQueuedModelList->model->vertices->offset,
			sQueuedModelList->model->normals->offset,
			sQueuedModelList->model->UVs->offset
		};

		vkCmdBindVertexBuffers(
			cBuff,
			0,
			kPerVertexBufferCount,
			perVertexBuffers,
			perVertexBufferOffsets
		);

		//////////////////////////
		/// per instance stuff ///
		//////////////////////////
		enum {kPerInstanceBufferCount = 2};
		VkBuffer perInstanceBuffers[kPerInstanceBufferCount] = {
			renderer->perInstanceVertexGPUBuffer->buffer,
			renderer->perInstanceVertexGPUBuffer->buffer,
		};

		size_t samplerOffset = 0;
		size_t matrixOffset = renderer->matrixStartIndex * sizeof(uint32_t);

		// these will be modified by the queuedRender's instance offset
		VkDeviceSize perInstanceBufferOffsets[kPerInstanceBufferCount] = {
			samplerOffset,
			matrixOffset,
		};

		vkCmdBindVertexBuffers(
			cBuff,
			kPerVertexBufferCount, // start where per vertex data ended
			kPerInstanceBufferCount,
			perInstanceBuffers,
			perInstanceBufferOffsets
		);

		vkCmdBindIndexBuffer(
			cBuff,
			sQueuedModelList->model->indices->src->buffer,
			sQueuedModelList->model->indices->offset,
			VK_INDEX_TYPE_UINT32
		);

		vkCmdDrawIndexed(
			cBuff,
			sQueuedModelList->model->indexCount,
			sQueuedModelList->instanceCount,
			0, // firstIndex
			0, // vertexOffset
			sQueuedModelList->instanceOffset // firstInstance
		);

		QueuedRender* old = sQueuedModelList;
		sQueuedModelList = sQueuedModelList->next;

		SDL_free(old);
	}

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
	////////////////////////////////////
	/// split into batches if needed ///
	////////////////////////////////////
	while(batchCount >= renderer->samplerMatrixArrayLength)
	{
		batchCount -= renderer->samplerMatrixArrayLength;
		VSR_RenderModels(
			renderer,
			model,
			transforms,
			samplers,
			renderer->samplerMatrixArrayLength
		);
	}

	QueuedRender* queuedRender = SDL_malloc(sizeof(QueuedRender));
	queuedRender->instanceOffset = renderer->modelInstanceCount;
	queuedRender->instanceCount = batchCount;
	queuedRender->model = model;
	queuedRender->next = NULL;

	renderer->modelInstanceCount += queuedRender->instanceCount;

	size_t lastInstanceIndex;
	if(sQueuedModelList)
	{
		QueuedRender* last = sQueuedModelList;
		while (last->next != NULL)
		{
			last = last->next;
		}

		last->next = queuedRender;
		lastInstanceIndex = last->instanceOffset + lastInstanceIndex;
	}
	else
	{
		sQueuedModelList = queuedRender;
		lastInstanceIndex = 0;
	}

	////////////////
	/// Matrices ///
	////////////////
	size_t  matrixTransferListSize = 4;
	size_t  matrixTransferListUsed = 0;

	size_t* matrixTransferStartIndexList = SDL_malloc(
		matrixTransferListSize * sizeof(size_t)
	);

	size_t* matrixTransferCountsList = SDL_malloc(
		matrixTransferListSize * sizeof(size_t)
	);

	for(size_t i = 0; i < batchCount; i++)
	{
		size_t mat4Index = lastInstanceIndex + renderer->matrixStartIndex + i;

		if(renderer->modelSamplerMatrixArray[mat4Index] != transforms[i]->uuid
		|| transforms[i]->needsUpdate == SDL_TRUE)
		{
			// track were the data is (compiler optimised?
			if(renderer->modelSamplerMatrixArray[mat4Index] != transforms[i]->uuid)
			{
				renderer->modelSamplerMatrixArray[mat4Index] = transforms[i]->uuid;
			}
			if(matrixTransferListUsed != 0)
			{
				size_t nextFromLast =
				matrixTransferStartIndexList[matrixTransferListUsed-1]
				+ matrixTransferCountsList[matrixTransferListUsed-1];

				if(nextFromLast == i)
				{
					matrixTransferCountsList[matrixTransferListUsed-1]++;
				}
			}
			else
			{
				matrixTransferStartIndexList[matrixTransferListUsed] = i;
				matrixTransferCountsList[matrixTransferListUsed++] = 1;
			}



			if(matrixTransferListUsed == matrixTransferListSize)
			{
				matrixTransferListSize = matrixTransferListSize * 2;

				matrixTransferStartIndexList = SDL_realloc(
					matrixTransferStartIndexList,
					matrixTransferListSize * sizeof(size_t)
					);

				matrixTransferCountsList = SDL_realloc(
					matrixTransferCountsList,
					matrixTransferListSize * sizeof(size_t)
				);
			}

			// this is going to be uploaded so its being 'updated'
			transforms[i]->needsUpdate = SDL_FALSE;
		}
	}

	////////////////
	/// Samplers ///
	////////////////
	size_t  samplerTransferListSize = 4;
	size_t  samplerTransferListUsed = 0;

	size_t* samplerTransferStartIndexList = SDL_malloc(
		matrixTransferListSize * sizeof(size_t)
	);

	size_t* samplerTransferCountsList = SDL_malloc(
		matrixTransferListSize * sizeof(size_t)
	);

	for(size_t i = 0; i < batchCount; i++)
	{
		size_t samplerIndex = lastInstanceIndex + i;

		if (renderer->modelSamplerMatrixArray[samplerIndex] != samplers[i]->uuid
		    || transforms[i]->needsUpdate == SDL_TRUE)
		{
			// track were the data is (compiler optimised?
			if (renderer->modelSamplerMatrixArray[samplerIndex] != samplers[i]->uuid)
			{
				renderer->modelSamplerMatrixArray[samplerIndex] = samplers[i]->uuid;
			}

			if (samplerTransferListUsed != 0)
			{
				size_t nextFromLast =
					samplerTransferStartIndexList[samplerTransferListUsed - 1]
					+ samplerTransferCountsList[samplerTransferListUsed - 1];

				if (nextFromLast == i)
				{
					samplerTransferCountsList[samplerTransferListUsed - 1]++;
				}
			}
			else
			{
				samplerTransferStartIndexList[samplerTransferListUsed] = i;
				samplerTransferCountsList[samplerTransferListUsed++] = 1;
			}


			if (samplerTransferListUsed == samplerTransferListSize)
			{
				samplerTransferListSize = samplerTransferListSize * 2;

				samplerTransferStartIndexList = SDL_realloc(
					samplerTransferStartIndexList,
					samplerTransferListSize * sizeof(size_t)
				);

				samplerTransferCountsList = SDL_realloc(
					samplerTransferCountsList,
					samplerTransferListSize * sizeof(size_t)
				);
			}

			// this is going to be uploaded so its being 'updated'
			samplers[i]->needsUpdate = SDL_FALSE;
		}
	}


	size_t transfersNeeded = samplerTransferListUsed + matrixTransferListUsed;

	VkBufferCopy* bufferCopy = SDL_malloc(
		transfersNeeded * sizeof(VkBufferCopy)
	);
	size_t mat4Size =  sizeof(float[16]);
	size_t samplerSize = sizeof(uint32_t);

	for(size_t i = 0; i < samplerTransferListUsed; i++)
	{
		size_t copySize = samplerTransferCountsList[i] * samplerSize;
		size_t dstOffset = lastInstanceIndex * samplerSize;

		Renderer_MemoryAlloc* alloc = Renderer_MemoryAllocate(
			renderer,
			renderer->vertexStagingBuffer,
			copySize,
			0
		);

		uint8_t* p = Renderer_MemoryAllocMap(renderer, alloc);
		for(size_t j = 0; j < samplerTransferCountsList[i]; j++)
		{
			size_t index = samplerTransferStartIndexList[i] + j;
			SDL_memcpy(p, &samplers[index]->textureIndex, samplerSize);
			p += samplerSize;
		}
		Renderer_MemoryAllocUnmap(renderer, alloc);

		Renderer_MemoryTransfer(
			renderer,
			renderer->perInstanceVertexGPUBuffer,
			dstOffset,
			alloc->src,
			alloc->offset,
			alloc->size,
			NULL
		);

		Renderer_MemoryAllocFree(renderer, alloc);
	}

	for(size_t i = 0; i < matrixTransferListUsed; i++)
	{
		size_t copySize = matrixTransferCountsList[i] * mat4Size;
		size_t samplerOffset = renderer->matrixStartIndex * samplerSize;
		size_t dstOffset = samplerOffset+(lastInstanceIndex * mat4Size);

		Renderer_MemoryAlloc* alloc = Renderer_MemoryAllocate(
			renderer,
			renderer->vertexStagingBuffer,
			copySize,
			0
		);

		uint8_t* p = Renderer_MemoryAllocMap(renderer, alloc);
		for(size_t j = 0; j < matrixTransferCountsList[i]; j++)
		{
			size_t index = matrixTransferStartIndexList[i] + j;
			VSR_Mat4* mat4 = transforms[index];
			SDL_memcpy(p, &mat4->m0, mat4Size);
			p += mat4Size;
		}
		Renderer_MemoryAllocUnmap(renderer, alloc);

		Renderer_MemoryTransfer(
			renderer,
			renderer->perInstanceVertexGPUBuffer,
			dstOffset,
			alloc->src,
			alloc->offset,
			alloc->size,
			NULL
			);

		Renderer_MemoryAllocFree(renderer, alloc);
	}

	return 0;
}





//==============================================================================
// VSR_RendererSetVertexConstants
//------------------------------------------------------------------------------
void
VSR_RendererSetVertexConstants(
	VSR_Renderer* renderer,
	VSR_PushConstants const* pushConstants)
{
	renderer->pushConstantsVertex = *pushConstants;
}





//==============================================================================
// VSR_RendererSetFragmentConstants
//------------------------------------------------------------------------------
void
VSR_RendererSetFragmentConstants(
	VSR_Renderer* renderer,
	VSR_PushConstants const* pushConstants)
{
	renderer->pushConstantsFragment = *pushConstants;
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

#include "VSR_Renderer.h"
#include "Renderer_Memory.h"





//==============================================================================
// findMemoryType
//------------------------------------------------------------------------------
uint32_t findMemoryType(
	VSR_Renderer* renderer,
	uint32_t typeFilter,
	VkMemoryPropertyFlags propertiesReq,
	size_t memRequired)
{
	VkPhysicalDeviceMemoryProperties memProperties;

	vkGetPhysicalDeviceMemoryProperties(
		renderer->physicalDevice.device,
		&memProperties);

	uint32_t index = -1;

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		VkMemoryPropertyFlags properties = memProperties.memoryTypes[i].propertyFlags;
		size_t size = memProperties.memoryHeaps[i].size;

		if ((typeFilter & (1 << i))
		&& (properties & propertiesReq) == propertiesReq
		&& size >= memRequired)
		{
			index = i;
			break;
		}
	}

	return index;
}





//==============================================================================
// createBuffer
//------------------------------------------------------------------------------
void
createBuffer(
	VSR_Renderer* renderer,
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer* buffer,
	VkDeviceMemory* bufferMemory)
{
	VkBufferCreateInfo bufferInfo = (VkBufferCreateInfo) {0};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size  = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateBuffer(
		renderer->logicalDevice.device,
		&bufferInfo,
		VSR_GetAllocator(),
		buffer
	);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(
		renderer->logicalDevice.device,
		*buffer,
		&memRequirements
	);

	size_t heapIndex = findMemoryType(
		renderer,
		memRequirements.memoryTypeBits,
		properties,
		size
	);

	if(heapIndex == -1)
	{
		goto FAIL;
	}

	VkMemoryAllocateInfo allocInfo = (VkMemoryAllocateInfo) {0};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = heapIndex;

	vkAllocateMemory(
		renderer->logicalDevice.device,
		&allocInfo,
		VSR_GetAllocator(),
		bufferMemory);

	vkBindBufferMemory(
		renderer->logicalDevice.device,
		*buffer,
		*bufferMemory,
		0);

	SUCCESS:
	return;

	FAIL:
	buffer = NULL;
}





//==============================================================================
// Renderer_MemoryCreate
//------------------------------------------------------------------------------
Renderer_Memory*
Renderer_MemoryCreate(
	VSR_Renderer* renderer,
	VkDeviceSize size,
	VkBufferUsageFlagBits use,
	VkMemoryPropertyFlags flags)
{

	VkBuffer buffer;
	VkDeviceMemory memory;

	createBuffer(
		renderer,
		size,
		use,
		flags,
		&buffer,
		&memory);

	if(buffer != NULL)
	{
		Renderer_Memory* rendererMemory = SDL_malloc(sizeof(Renderer_Memory));

		rendererMemory->buffer = buffer;
		rendererMemory->memory = memory;

		VkFenceCreateInfo fenceInfo;
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.pNext = NULL;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		vkCreateFence(
			renderer->logicalDevice.device,
			&fenceInfo,
			VSR_GetAllocator(),
			&rendererMemory->bufferFence);

		rendererMemory->bufferSize = size;
		rendererMemory->root = NULL;
		return rendererMemory;
	}
	else
	{
		return NULL;
	}
}





//==============================================================================
// Renderer_MemoryAllocFree
//------------------------------------------------------------------------------
void
Renderer_MemoryDestroy(
	VSR_Renderer* renderer,
	Renderer_Memory* memory)
{
	vkWaitForFences(
		renderer->logicalDevice.device,
		1,
		&memory->bufferFence,
		VK_TRUE,
		-1
	);

	vkDestroyFence(
		renderer->logicalDevice.device,
		memory->bufferFence,
		VSR_GetAllocator());

	vkDestroyBuffer(
		renderer->logicalDevice.device,
		memory->buffer,
		VSR_GetAllocator());

	vkFreeMemory(
		renderer->logicalDevice.device,
		memory->memory,
		VSR_GetAllocator());

	SDL_free(memory);
}





//==============================================================================
// Renderer_MemoryReset
//------------------------------------------------------------------------------
void
Renderer_MemoryReset(
	Renderer_Memory* memory)
{
	memory->root = NULL;
}





//==============================================================================
// Renderer_MemoryTransfer
//------------------------------------------------------------------------------
int
Renderer_MemoryTransfer(
	VSR_Renderer* renderer,
	Renderer_Memory* dst,
	VkDeviceSize dstOffset,
	Renderer_Memory* src,
	VkDeviceSize srcOffset,
	VkDeviceSize len,
	VkFence fence)
{

	VSR_GenerationalFence genFence;

	VkCommandBuffer buff = Renderer_CommandPoolAllocateTransferBuffer(
		renderer,
		&genFence);

	size_t genAcquired = *genFence.generation;

	VkBufferCopy copyRegion = (VkBufferCopy){0};
	copyRegion.dstOffset = dstOffset;
	copyRegion.srcOffset = srcOffset;
	copyRegion.size = len;

	vkCmdCopyBuffer(
		buff,
		src->buffer,
		dst->buffer,
		1,
		&copyRegion);

	Renderer_CommandPoolSubmitTransferBuffer(
		renderer,
		buff
	);

	Renderer_WaitOnGenerationalFence(
		renderer,
		1,
		&genFence,
		genAcquired
	);

	return 0;
}





//==============================================================================
// Renderer_MemoryTransferAlloc
//------------------------------------------------------------------------------
int
Renderer_MemoryTransferAlloc(
	VSR_Renderer* renderer,
	Renderer_MemoryAlloc* dst,
	Renderer_MemoryAlloc* src)
{

	int res = Renderer_MemoryTransfer(
		renderer,
		dst->src,
		dst->offset,
		src->src,
		src->offset,
		src->size,
		NULL
	);

	return res;
}





//==============================================================================
// Renderer_MemoryTransferToImage
//------------------------------------------------------------------------------
int
Renderer_MemoryTransferToImage(
	VSR_Renderer* renderer,
	const Renderer_MemoryAlloc* src,
	VSR_Image* dist
)
{
	VSR_GenerationalFence fence;

	VkCommandBuffer buff = Renderer_CommandPoolAllocateTransferBuffer(
		renderer,
		&fence
	);

	size_t generationAcquired = *fence.generation;

	VkBufferImageCopy imageCopy;
	imageCopy.bufferOffset = src->offset;
	imageCopy.bufferImageHeight =  dist->src.h;
	imageCopy.bufferRowLength = dist->src.w;
	imageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopy.imageSubresource.mipLevel = 0;
	imageCopy.imageSubresource.baseArrayLayer = 0;
	imageCopy.imageSubresource.layerCount = 1;
	imageCopy.imageOffset = (VkOffset3D){0,0,0};
	imageCopy.imageExtent.width = dist->src.w;
	imageCopy.imageExtent.height = dist->src.h;
	imageCopy.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(
		buff,
		src->src->buffer,
		dist->image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&imageCopy
	);

	Renderer_CommandPoolSubmitTransferBuffer(
		renderer,
		buff
	);

	Renderer_WaitOnGenerationalFence(renderer, 1, &fence, generationAcquired);

	return SDL_TRUE;
}





//==============================================================================
// Renderer_MemoryAllocate
//------------------------------------------------------------------------------
Renderer_MemoryAlloc*
Renderer_MemoryAllocate(
	VSR_Renderer* renderer,
	Renderer_Memory* memory,
	VkDeviceSize size,
	VkDeviceSize align)
{
	Renderer_MemoryAlloc* alloc = NULL;

	// get the root allocation
	Renderer_MemoryAlloc* runner = memory->root;

	// no alloc in mem
	if(!runner)
	{ // init with this alloc as the root (first alloc)
		if(memory->bufferSize >= size)
		{ //
			alloc = SDL_calloc(1, sizeof(Renderer_MemoryAlloc));
			alloc->prev = NULL;
			alloc->next = NULL;
			alloc->offset = 0;
			alloc->size = size;
			alloc->align = align;
			memory->root = alloc;
			// don't set runner so we can skip the find as we've found one
		}
	}
	else
	{ // we have data in this

		while(runner->next != NULL)
		{
			runner = runner->next;
		}

		size_t freeBegin = runner->offset + runner->size;
		if(align)
		{
			freeBegin = ((freeBegin + align-1) / align) * align;
		}

		if((freeBegin + size) <= memory->bufferSize)
		{
			alloc = SDL_calloc(1, sizeof(Renderer_MemoryAlloc));

			runner->next = alloc;
			alloc->prev = runner;

			alloc->next = NULL;
			alloc->offset = freeBegin;
			alloc->size = size;
			alloc->align = align;
			memory->root = alloc;
		}

	}


	if(alloc)
	{
		// remember where you came from
		alloc->src = memory;
	}
	return alloc;
}

//==============================================================================
// Renderer_MemoryAllocFree
//------------------------------------------------------------------------------
void
Renderer_MemoryAllocFree(
	VSR_Renderer* renderer,
	Renderer_MemoryAlloc* alloc)
{
	if(alloc)
	{


		if (alloc->prev)
		{
			alloc->prev->next = alloc->next;
			alloc->prev = NULL;
		}

		if (alloc->next)
		{
			alloc->next->prev = alloc->prev;
			alloc->next = NULL;
		}

		if(alloc->src->root == alloc)
		{
			alloc->src->root = NULL;
		}
		SDL_free(alloc);
	}
}


//==============================================================================
// Renderer_MemoryAllocMap
//------------------------------------------------------------------------------
void*
Renderer_MemoryAllocMap(
	VSR_Renderer* renderer,
	Renderer_MemoryAlloc* alloc)
{
	void* p = NULL;

	VkMemoryMapFlags flags = 0L; // currently unimp'd

	vkMapMemory(
		renderer->logicalDevice.device,
		alloc->src->memory,
		alloc->offset,
		alloc->size,
		flags,
		&p);

	return p;
}





//==============================================================================
// Renderer_MemoryAllocUnmap
//------------------------------------------------------------------------------
void
Renderer_MemoryAllocUnmap(
	VSR_Renderer* renderer,
	Renderer_MemoryAlloc* alloc)
{
	vkUnmapMemory(
		renderer->logicalDevice.device,
		alloc->src->memory);
}
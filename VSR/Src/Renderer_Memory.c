#include "VSR_Renderer.h"
#include "Renderer_Memory.h"





//==============================================================================
// findMemoryType
//------------------------------------------------------------------------------
uint32_t findMemoryType(
	VSR_Renderer* renderer,
	uint32_t typeFilter,
	VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;

	vkGetPhysicalDeviceMemoryProperties(
		renderer->subStructs->physicalDevice.device,
		&memProperties);

	uint32_t index = -1;

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i))
		&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
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
		renderer->subStructs->logicalDevice.device,
		&bufferInfo,
		VSR_GetAllocator(),
		buffer);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(
		renderer->subStructs->logicalDevice.device,
		*buffer,
		&memRequirements);

	VkMemoryAllocateInfo allocInfo = (VkMemoryAllocateInfo) {0};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(
		renderer,
		memRequirements.memoryTypeBits,
		properties);

	vkAllocateMemory(
		renderer->subStructs->logicalDevice.device,
		&allocInfo,
		VSR_GetAllocator(),
		bufferMemory);

	vkBindBufferMemory(
		renderer->subStructs->logicalDevice.device,
		*buffer,
		*bufferMemory,
		0);
}





//==============================================================================
// Renderer_MemoryCreate
//------------------------------------------------------------------------------
Renderer_Memory
Renderer_MemoryCreate(
	VSR_Renderer* renderer,
	VkDeviceSize size,
	VkBufferUsageFlagBits use,
	VkMemoryPropertyFlags flags)
{
	// this will be copied out so alloc on stack
	Renderer_Memory rendererMemory = (Renderer_Memory){0};

	createBuffer(
		renderer,
		size,
		use,
		flags,
		&rendererMemory.buffer,
		&rendererMemory.memory);

	rendererMemory.bufferSize = size;
	rendererMemory.root = NULL;

	return rendererMemory;
}





//==============================================================================
// Renderer_MemoryFree
//------------------------------------------------------------------------------
void
Renderer_MemoryDestroy(
	VSR_Renderer* renderer,
	Renderer_Memory memory)
{
	vkDestroyBuffer(
		renderer->subStructs->logicalDevice.device,
		memory.buffer,
		VSR_GetAllocator());

	vkFreeMemory(
		renderer->subStructs->logicalDevice.device,
		memory.memory,
		VSR_GetAllocator());
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
	Renderer_Memory dst,
	VkDeviceSize dstOffset,
	Renderer_Memory src,
	VkDeviceSize srcOffset,
	VkDeviceSize len)
{
	VkCommandBuffer buff = GraphicsPipeline_CommandPoolAllocateTransferBuffer(
		renderer,
		renderer->subStructs->pipeline
	);

	VkBufferCopy copyRegion = (VkBufferCopy){0};
	copyRegion.dstOffset = dstOffset;
	copyRegion.srcOffset = srcOffset;
	copyRegion.size = len;

	vkCmdCopyBuffer(buff, src.buffer, dst.buffer, 1, &copyRegion);

	GraphicsPipeline_CommandPoolSubmitTransferBuffer(
		renderer,
		renderer->subStructs->pipeline,
		buff
	);


	return 0;
}


int
Renderer_MemoryTransferToImage(
	VSR_Renderer* renderer,
	const Renderer_MemoryAlloc* src,
	VSR_Image* dist
)
{
	VkCommandBuffer buff = GraphicsPipeline_CommandPoolAllocateTransferBuffer(
		renderer,
		renderer->subStructs->pipeline
	);

	VkBufferImageCopy imageCopy;
	imageCopy.bufferOffset = src->offset;
	imageCopy.bufferImageHeight = 0;
	imageCopy.bufferRowLength = 0;
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
		renderer->subStructs->USDStagingBuffer.buffer,
		dist->image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&imageCopy
	);

	GraphicsPipeline_CommandPoolSubmitTransferBuffer(
		renderer,
		renderer->subStructs->pipeline,
		buff
	);

	return SDL_TRUE;
}


//==============================================================================
// Renderer_MemoryAllocate
//------------------------------------------------------------------------------
Renderer_MemoryAlloc*
Renderer_MemoryAllocate(
	VSR_Renderer* renderer,
	Renderer_Memory* memory,
	VkDeviceSize size)
{
	Renderer_MemoryAlloc* alloc = NULL;

	// get the root allocation
	Renderer_MemoryAlloc* runner = memory->root;

	// no alloc in mem
	if(!runner)
	{ // init with this alloc as the root (first alloc)

		if(memory->bufferSize >= size)
		{
			alloc = SDL_malloc(sizeof(Renderer_MemoryAlloc));
			alloc->prev = NULL;
			alloc->next = NULL;
			alloc->offset = 0;
			alloc->size = size;

			memory->root = alloc;
			// don't set runner so we can skip the find as we've found one
		}
	}

	// track how much memory we'd free after a defrag
	VkDeviceSize possibleContiguous = 0;
	while (runner)
	{
		VkDeviceSize endOfRunnerOffset = runner->offset + runner->size;
		VkDeviceSize spaceTillNext;

		if (!runner->next)
		{
			spaceTillNext = memory->bufferSize - endOfRunnerOffset;
		} else
		{
			spaceTillNext = runner->next->offset - endOfRunnerOffset;
		}

		// we got one
		if (spaceTillNext >= size)
		{
			// un-null the alloc
			alloc = SDL_malloc(sizeof(Renderer_MemoryAlloc));

			// if there was a next
			if (runner->next)
			{ // set their prev to alloc
				runner->next->prev = alloc;
			}
			// join allloc to runner next
			runner->next = alloc;

			alloc->prev   = runner;
			alloc->next   = runner->next;
			alloc->offset = endOfRunnerOffset;
			alloc->size   = size;
			break;
		}

		// otherwise continue and track any space found
		possibleContiguous += spaceTillNext;
		runner = runner->next;

	}

	if(!alloc 	// if no valid block was found
	   && possibleContiguous >= size) // BUT - we can make one
	{
		// TODO: defrag

	}

	return alloc;
}

//==============================================================================
// Renderer_MemoryFree
//------------------------------------------------------------------------------
void
Renderer_MemoryFree(
	VSR_Renderer* renderer,
	Renderer_MemoryAlloc* alloc)
{
	if(alloc)
	{
		if (alloc->prev)
		{
			alloc->prev->next = alloc->next;
		}

		if (alloc->next)
		{
			alloc->next->prev = alloc->prev;
		}

		SDL_free(alloc);
	}
}


//==============================================================================
// Render_MemoryMapAlloc
//------------------------------------------------------------------------------
void*
Render_MemoryMapAlloc(
	VSR_Renderer* renderer,
	Renderer_Memory mem,
	Renderer_MemoryAlloc* alloc)
{
	void* p = NULL;

	VkMemoryMapFlags flags = 0L; // currently unimp'd

	vkMapMemory(
		renderer->subStructs->logicalDevice.device,
		mem.memory,
		alloc->offset,
		alloc->size,
		flags,
		&p);

	return p;
}





//==============================================================================
// Render_MemoryUnmapAlloc
//------------------------------------------------------------------------------
void
Render_MemoryUnmapAlloc(
	VSR_Renderer* renderer,
	Renderer_Memory mem)
{
	vkUnmapMemory(
		renderer->subStructs->logicalDevice.device,
		mem.memory);
}
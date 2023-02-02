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
	Renderer_Memory rendererMemory = (Renderer_Memory){0};

	createBuffer(
		renderer,
		size,
		use,
		flags,
		&rendererMemory.buffer,
		&rendererMemory.memory);

	rendererMemory.bufferSize = size;
	rendererMemory.lastUsed = 0;


	return rendererMemory;
}





//==============================================================================
// Renderer_MemoryFree
//------------------------------------------------------------------------------
void
Renderer_MemoryFree(
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
	memory->lastUsed = 0;
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
		renderer->subStructs->pipeline);

	VkCommandBufferBeginInfo beginInfo = (VkCommandBufferBeginInfo){0};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(buff, &beginInfo);
	VkBufferCopy copyRegion = (VkBufferCopy){0};
	copyRegion.dstOffset = dstOffset;
	copyRegion.srcOffset = srcOffset;
	copyRegion.size = len;

	vkCmdCopyBuffer(buff, src.buffer, dst.buffer, 1, &copyRegion);
	vkEndCommandBuffer(buff);

	VkSubmitInfo submitInfo = (VkSubmitInfo){0};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buff;

	VkFence transferFence;
	VkFenceCreateInfo fenceInfo;
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = NULL;
	fenceInfo.flags = 0;

	vkCreateFence(
		renderer->subStructs->logicalDevice.device,
		&fenceInfo,
		VSR_GetAllocator(),
		&transferFence);

	vkQueueSubmit(
		renderer->subStructs->deviceQueues.transferQueue,
		1,
		&submitInfo,
		transferFence);

	vkWaitForFences(
		renderer->subStructs->logicalDevice.device,
		1,
		&transferFence,
		VK_TRUE,
		-1);

	vkDestroyFence(
		renderer->subStructs->logicalDevice.device,
		transferFence,
		VSR_GetAllocator());

	return 0;
}





//==============================================================================
// Renderer_MemoryAllocate
//------------------------------------------------------------------------------
Renderer_MemoryAlloc
Renderer_MemoryAllocate(
	VSR_Renderer* renderer,
	Renderer_Memory* memory,
	VkDeviceSize size)
{
	Renderer_MemoryAlloc alloc = (Renderer_MemoryAlloc){0};

	alloc.offset = memory->lastUsed;
	alloc.size = size;

	memory->lastUsed += size;

	return alloc;
}





//==============================================================================
// Render_MemoryMapAlloc
//------------------------------------------------------------------------------
void*
Render_MemoryMapAlloc(
	VSR_Renderer* renderer,
	Renderer_Memory mem,
	Renderer_MemoryAlloc alloc)
{
	void* p = NULL;

	VkMemoryMapFlags flags = 0L; // currently unimp'd

	vkMapMemory(
		renderer->subStructs->logicalDevice.device,
		mem.memory,
		alloc.offset,
		alloc.size,
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
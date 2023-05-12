#ifndef RENDERER_MEMORY_H
#define RENDERER_MEMORY_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>

////////////////////////////////
/// Renderer pre-declaration ///
////////////////////////////////
typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs;

/////////////////////////////
/// Image pre-declaration ///
/////////////////////////////
typedef struct VSR_Image VSR_Image;
struct VSR_Image;

///////////////////////////////////////
/// Renderer_Memory pre-declaration ///
///////////////////////////////////////
typedef struct Renderer_Memory Renderer_Memory;

typedef struct Renderer_MemoryAlloc Renderer_MemoryAlloc;
struct Renderer_MemoryAlloc
{
	VkSemaphore bufferInUseSignal;

	Renderer_MemoryAlloc* prev;
	Renderer_MemoryAlloc* next;

	Renderer_Memory* src;
	VkDeviceSize   offset;
	VkDeviceSize   size;
	VkDeviceSize   align;
};


struct Renderer_Memory
{
	VkFence bufferFence;

	VkDeviceMemory memory;
	VkBuffer       buffer;
	VkDeviceSize   bufferSize; // buffer is 1:1 with memory

	Renderer_MemoryAlloc* root;
};

Renderer_Memory*
Renderer_MemoryCreate(
	VSR_Renderer* renderer,
	VkDeviceSize size,
	VkBufferUsageFlagBits use,
	VkMemoryPropertyFlags flags);

void
Renderer_MemoryDestroy(
	VSR_Renderer* renderer,
	Renderer_Memory* memory);

void
Renderer_MemoryReset(
	Renderer_Memory* memory);

int
Renderer_MemoryTransfer(
	VSR_Renderer* renderer,
	Renderer_Memory* dst,
	VkDeviceSize dstOffset,
	Renderer_Memory* src,
	VkDeviceSize srcOffset,
	VkDeviceSize len,
	VkFence fence);

int
Renderer_MemoryTransferAlloc(
	VSR_Renderer* renderer,
	Renderer_MemoryAlloc* dst,
	Renderer_MemoryAlloc* src);

int
Renderer_MemoryTransferToImage(
	VSR_Renderer* renderer,
	const Renderer_MemoryAlloc* src,
	VSR_Image* dist
	);

Renderer_MemoryAlloc*
Renderer_MemoryAllocate(
	VSR_Renderer* renderer,
	Renderer_Memory* memory,
	VkDeviceSize size,
	VkDeviceSize align);

void
Renderer_MemoryAllocFree(
	VSR_Renderer* renderer,
	Renderer_MemoryAlloc* alloc);

void*
Renderer_MemoryAllocMap(
	VSR_Renderer* renderer,
	Renderer_MemoryAlloc* alloc);

void
Renderer_MemoryAllocUnmap(
	VSR_Renderer* renderer,
	Renderer_MemoryAlloc* alloc);


#endif //RENDERER_MEMORY_H

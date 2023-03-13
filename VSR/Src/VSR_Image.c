#include "VSR_Image.h"

#include "VSR_Renderer.h"
#include "VSR_error.h"

VkImage createImage(VSR_Renderer* renderer,
	   uint32_t w,
	   uint32_t h,
	   VkFormat format,
	   VkImageTiling tiling,
	   VkImageUsageFlags usage)
{
	VkImageCreateInfo imageCreateInfo;

	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.flags = 0L;

	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = w;
	imageCreateInfo.extent.height = h;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = format;
	imageCreateInfo.tiling = tiling;
	imageCreateInfo.usage = usage;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkImage image;

	vkCreateImage(
		renderer->subStructs->logicalDevice.device,
		&imageCreateInfo,
		VSR_GetAllocator(),
		&image
	);

	return image;
}


//==============================================================================
// VSR_ImageCreate
//------------------------------------------------------------------------------
VSR_Image*
VSR_ImageCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	SDL_Surface* surface,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags useFlags)
{
	VSR_Image* image = SDL_malloc(sizeof(VSR_Image));

	image->src = *surface;
	size_t surfaceExtent = (surface->w * surface->h);
	size_t imageBufferSize;

	// it's possible we've just passed the surface only fo wh
	if(surface->format)
	{
		imageBufferSize = surface->format->BytesPerPixel * surfaceExtent;
	}
	else
	{
		imageBufferSize = 4 * surfaceExtent;
	}

	if(surface->pixels)
	{
		surface = SDL_ConvertSurfaceFormat(
			surface,
			SDL_PIXELFORMAT_ABGR8888,
			0);
	}

	///////////////////////////////
	/// create memory for image ///
	///////////////////////////////

	image->alloc = Renderer_MemoryAllocate(
		renderer,
		&renderer->subStructs->USDGPUBuffer,
		imageBufferSize
	);

	image->format = format;
	image->image = createImage(
		renderer,
		surface->w,
		surface->h,
		image->format,
		tiling,
		useFlags
	);

	vkBindImageMemory(
		renderer->subStructs->logicalDevice.device,
		image->image,
		image->alloc->src->memory,
		image->alloc->offset
	);

	// if there is surface data, copy it to the image
	if(surface->pixels)
	{
		//////////////////////////////////////////
		/// transition image ready for writing ///
		//////////////////////////////////////////
		VSR_ImageTransition(
			renderer,
			pipeline,
			image,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		);

		/////////////////////////////////
		/// write pixel data to stage ///
		/////////////////////////////////

		// grab a staging buffer
		Renderer_MemoryAlloc* alloc = Renderer_MemoryAllocate(
			renderer,
			&renderer->subStructs->USDStagingBuffer,
			image->alloc->size
		);

		void* p = Render_MemoryMapAlloc(
			renderer,
			alloc
		);

		SDL_LockSurface(surface);
		memcpy(p, surface->pixels, alloc->size);
		SDL_UnlockSurface(surface);

		Render_MemoryUnmapAlloc(
			renderer,
			alloc
		);

		///////////////////////////////
		/// blit stage to dst image ///
		///////////////////////////////
		Renderer_MemoryTransferToImage(
			renderer,
			alloc,
			image
		);

		///////////////////////////
		/// free staging buffer ///
		///////////////////////////
	//	Renderer_MemoryFree(renderer, alloc);

	}

	return image;
}





//==============================================================================
// VSR_ImageViewCreate
//------------------------------------------------------------------------------
void
VSR_ImageTransition(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_Image* img,
	VkImageLayout from,
	VkImageLayout to)
{
	VkCommandBuffer buff = GraphicsPipeline_CommandPoolAllocateTransferBuffer(
		renderer,
		pipeline);

	VkImageMemoryBarrier imageBarrier = (VkImageMemoryBarrier){0};
	imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageBarrier.oldLayout = from;
	imageBarrier.newLayout = to;
	imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.image = img->image;
	imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBarrier.subresourceRange.baseMipLevel = 0;
	imageBarrier.subresourceRange.levelCount = 1;
	imageBarrier.subresourceRange.baseArrayLayer = 0;
	imageBarrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlagBits srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlagBits dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	if(to == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		imageBarrier.srcAccessMask = 0;
		imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if(to == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}



	vkCmdPipelineBarrier(
		buff,
		srcStage, dstStage,0,
		0,NULL,
		0,NULL,
		1, &imageBarrier
	);

	GraphicsPipeline_CommandPoolSubmitTransferBuffer(renderer,pipeline,buff);
}




//==============================================================================
// VSR_ImageViewCreate
//------------------------------------------------------------------------------
VSR_ImageView
VSR_ImageViewCreate(
	VSR_Renderer* renderer,
	VkImage image,
	VkFormat imageFormat,
	VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewCreateInfo;

	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.flags = 0;
	viewCreateInfo.pNext = NULL;
	viewCreateInfo.image = image;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = imageFormat;
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	viewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;

	VkResult err;
	VkImageView view;

	err = vkCreateImageView(
		renderer->subStructs->logicalDevice.device,
		&viewCreateInfo,
		VSR_GetAllocator(),
		&view
		);

	VSR_ImageView imageView;
	if(err == VK_SUCCESS)
	{
		imageView.image     = image;
		imageView.format    = imageFormat;
		imageView.imageView = view;
	}

	return imageView;
}





//==============================================================================
// VSR_ImageViewDestroy
//------------------------------------------------------------------------------
void
VSR_ImageViewDestroy(
	VSR_Renderer* renderer,
	VSR_ImageView* imageView)
{
	vkDestroyImageView(renderer->subStructs->logicalDevice.device,
					   imageView->imageView, VSR_GetAllocator());

	VSR_ImageView blank = {.image = VK_NULL_HANDLE,
						   .format = VK_FORMAT_UNDEFINED,
						   .imageView = VK_NULL_HANDLE,
						   };
	*imageView = blank;
}
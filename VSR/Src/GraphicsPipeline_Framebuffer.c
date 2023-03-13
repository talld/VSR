#include "GraphicsPipeline_Framebuffer.h"

#include "VSR_Renderer.h"
#include "VSR_error.h"

SDL_bool
GraphicsPipeline_FramebufferPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* graphicsPipelineCreateInfo)
{
	return SDL_TRUE;
}


SDL_bool
GraphicsPipeline_FramebufferCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	VSR_GraphicsPipelineCreateInfo* createInfo)
{
	VkResult err;

	size_t frameCount = renderer->subStructs->swapchain.imageViewCount;
	size_t frameListSize = sizeof(VkFramebuffer) * frameCount;
	pipeline->subStructs->framebuffer.framebuffers = SDL_malloc(frameListSize);
	VkFramebuffer* frames = pipeline->subStructs->framebuffer.framebuffers;

	VkFramebufferCreateInfo* framebufferCreateInfo =
		&createInfo->subStructs->framebufferCreateInfo.framebufferCreateInfo;

	framebufferCreateInfo->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo->pNext = NULL;
	framebufferCreateInfo->flags = 0L;
	framebufferCreateInfo->renderPass = pipeline->subStructs->renderPass.renderPass;
	framebufferCreateInfo->attachmentCount = 2; // colour, depth
	VkImageView attachments[2] = {
		renderer->subStructs->swapchain.imageViews[0].imageView,
		pipeline->subStructs->depthView.imageView
		};

	framebufferCreateInfo->height = renderer->subStructs->surface.surfaceHeight;
	framebufferCreateInfo->width = renderer->subStructs->surface.surfaceWidth;
	framebufferCreateInfo->layers = 1;

	for(size_t i = 0; i < frameCount; i++)
	{
		attachments[0] = renderer->subStructs->swapchain.imageViews[i].imageView;
		// depth stencil doesn't change from image to image here
		framebufferCreateInfo->pAttachments = attachments;

		err = vkCreateFramebuffer(renderer->subStructs->logicalDevice.device,
								  framebufferCreateInfo,
								  VSR_GetAllocator(),
								  &frames[i]);

		if(err != VK_SUCCESS)
		{
            VSR_Error("Failed to create framebuffer: %s",
					VSR_VkErrorToString(err));
			goto FAIL;
		}
	}
	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}

void
GraphicsPipeline_FramebufferDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline
)
{
	size_t images = renderer->subStructs->swapchain.imageViewCount;
	for(size_t i = 0; i < images; i++)
	{
		vkDestroyFramebuffer(renderer->subStructs->logicalDevice.device,
							 pipeline->subStructs->framebuffer.framebuffers[i],
							 VSR_GetAllocator());
	}

	SDL_free(pipeline->subStructs->framebuffer.framebuffers);
}
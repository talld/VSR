#include "Renderer_Framebuffer.h"

#include "Renderer.h"
#include "VSR_error.h"

SDL_bool
VSR_FramebufferPopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs)
{
	return SDL_TRUE;
}


SDL_bool
VSR_FramebufferCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs)
{
	VkResult err;

	size_t frameCount = renderer->subStructs->swapchain.imageViewCount;
	size_t frameListSize = sizeof(VkFramebuffer) * frameCount;
	renderer->subStructs->framebuffer.framebuffers = SDL_malloc(frameListSize);
	VkFramebuffer* frames = renderer->subStructs->framebuffer.framebuffers;

	VkFramebufferCreateInfo* framebufferCreateInfo =
		&subStructs->framebufferCreateInfo.framebufferCreateInfo;

	framebufferCreateInfo->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo->pNext = NULL;
	framebufferCreateInfo->flags = 0L;
	framebufferCreateInfo->renderPass = renderer->subStructs->renderPass.renderPass;
	framebufferCreateInfo->attachmentCount = 1; // just colour for now
	framebufferCreateInfo->height = renderer->subStructs->surface.surfaceHeight;
	framebufferCreateInfo->width = renderer->subStructs->surface.surfaceWidth;
	framebufferCreateInfo->layers = 1;

	for(size_t i = 0; i < frameCount; i++)
	{
		VSR_ImageView swapchainImageView =
			renderer->subStructs->swapchain.imageViews[i];

		framebufferCreateInfo->pAttachments = &swapchainImageView.imageView;

		err = vkCreateFramebuffer(renderer->subStructs->logicalDevice.device,
								  framebufferCreateInfo,
								  VSR_GetAllocator(),
								  &frames[i]);

		if(err != VK_SUCCESS)
		{
			char errMsg[255];
			sprintf(errMsg, "Failed to create framebuffer: %s",
					VSR_VkErrorToString(err));

			VSR_SetErr(errMsg);
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
VSR_FramebufferDestroy(
	VSR_Renderer* renderer
)
{
	size_t images = renderer->subStructs->swapchain.imageViewCount;
	for(size_t i = 0; i < images; i++)
	{
		vkDestroyFramebuffer(renderer->subStructs->logicalDevice.device,
							 renderer->subStructs->framebuffer.framebuffers[i],
							 VSR_GetAllocator());
	}
}
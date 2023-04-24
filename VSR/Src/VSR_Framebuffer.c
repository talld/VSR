#include "VSR_Framebuffer.h"

#include "VSR_Renderer.h"
#include "VSR_error.h"

VSR_Framebuffer*
VSR_FramebufferCreate(
	VSR_Renderer* renderer,
	VSR_ImageView* imageView)
{
	VkResult err;

	VkFramebuffer frame;

	VkFramebufferCreateInfo framebufferCreateInfo;
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.pNext = NULL;
	framebufferCreateInfo.flags = 0L;
	framebufferCreateInfo.renderPass = renderer->renderPass.renderPass;
	framebufferCreateInfo.attachmentCount = 2; // colour, depth
	framebufferCreateInfo.height = renderer->surface.surfaceHeight;
	framebufferCreateInfo.width = renderer->surface.surfaceWidth;
	framebufferCreateInfo.layers = 1;

	VkImageView attachments[2] = {
		imageView->imageView,
		renderer->depthView->imageView
	};
	framebufferCreateInfo.pAttachments = attachments;

	err = vkCreateFramebuffer(
		renderer->logicalDevice.device,
		&framebufferCreateInfo,
		VSR_GetAllocator(),
		&frame
	);

	if(err != VK_SUCCESS)
	{
           VSR_Error("Failed to create framebuffer: %s",
				VSR_VkErrorToString(err));
		goto FAIL;
	}

	SUCCESS:
	{
		VSR_Framebuffer* framebuffer = SDL_malloc(sizeof(VSR_Framebuffer));
		framebuffer->frame = frame;
		return framebuffer;
	}

	FAIL:
	{
		return NULL;
	}
}

void
VSR_FramebufferDestroy(
	VSR_Renderer* renderer,
	VSR_Framebuffer* framebuffer)
{
	vkDestroyFramebuffer(
		renderer->logicalDevice.device,
		framebuffer->frame,
		VSR_GetAllocator()
	);

	SDL_free(framebuffer);
}
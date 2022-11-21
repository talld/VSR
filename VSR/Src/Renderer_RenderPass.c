#include "Renderer_RenderPass.h"

#include "Renderer.h"
#include "VSR_error.h"





//==============================================================================
// VSR_RendererPopulateGraphicsPipelineCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_RendererPopulateGraphicsPipelineCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs)
{
}





//==============================================================================
// VSR_CreateGraphicsPipeline
//------------------------------------------------------------------------------
SDL_bool
VSR_CreateGraphicsPipeline(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs)
{
	VkAttachmentDescription colourAttachment;
	colourAttachment.flags = 0L;
	colourAttachment.format = renderer->subStructs->surface.surfaceFormat;
	colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // dont care
	// sub-passes' layout transitions happen here
	colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // store

	//
	VkAttachmentReference colourAttachmentRef;
	colourAttachmentRef.layout = 0;
	colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = (VkSubpassDescription){0};
	subpass.flags = 0L;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colourAttachmentRef;

	VkRenderPassCreateInfo* passInfo;
	passInfo->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	passInfo->attachmentCount = 1;
	passInfo->pAttachments = &colourAttachment;
}
#include "GraphicsPipeline_RenderPass.h"

#include "VSR_Renderer.h"
#include "VSR_error.h"





//==============================================================================
// VSR_RenderPassPopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
GraphicsPipeline_RenderPassPopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo,
	GraphicsPipeline_CreateInfoSubStructs* subStructs)
{
	// TODO: move stuff here
	return SDL_TRUE;
}





//==============================================================================
// VSR_RenderPassCreate
//------------------------------------------------------------------------------
SDL_bool
GraphicsPipeline_RenderPassCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	GraphicsPipeline_CreateInfoSubStructs* subStructs)
{
	///////////////////
	/// colour pass ///
	///////////////////

	VkAttachmentDescription colourAttachment = (VkAttachmentDescription){0};
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

	// colour buffer pass (0)
	VkAttachmentReference colourAttachmentRef;
	colourAttachmentRef.attachment = 0;
	colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = (VkSubpassDescription){0};
	subpass.flags = 0L;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colourAttachmentRef;

	/////////////////////////////
	/// sub-pass dependencies ///
	/////////////////////////////

	// 1 + pass count ( 0 is external;
	VkSubpassDependency subpassDependencies[2];

	// colour buffer write
	subpassDependencies[0] = (VkSubpassDependency){0};
	subpassDependencies[0].dependencyFlags = 0;
	subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependencies[0].dstSubpass = 0;
	subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependencies[0].dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;

	// colour buffer present
	subpassDependencies[1] = (VkSubpassDependency){0};
	subpassDependencies[1].dependencyFlags = 0;
	subpassDependencies[1].srcSubpass = 0;
	subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependencies[1].srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
	subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	// read here is confusing, but it's technically a transfer op?

	///////////////////
	/// render-pass ///
	///////////////////

	VkRenderPassCreateInfo passInfo = (VkRenderPassCreateInfo){0};
	passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	passInfo.attachmentCount = 1;
	passInfo.pAttachments = &colourAttachment;
	passInfo.subpassCount = 1;
	passInfo.pSubpasses = &subpass;
	passInfo.dependencyCount = 2;
	passInfo.pDependencies = subpassDependencies;

	VkResult err = vkCreateRenderPass(
		renderer->subStructs->logicalDevice.device,
		&passInfo,
		VSR_GetAllocator(),
		&pipeline->subStructs->renderPass.renderPass);

	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create renderpass: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
		goto FAIL;
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





//==============================================================================
// VSR_RenderPassDestroy
//------------------------------------------------------------------------------
void
GraphicsPipeline_RenderPassDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline
)
{
	vkDestroyRenderPass(renderer->subStructs->logicalDevice.device,
						pipeline->subStructs->renderPass.renderPass,
						VSR_GetAllocator());
}
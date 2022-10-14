#include "Renderer_GraphicsPipeline.h"

#include "Renderer.h"


//==============================================================================
// VSR_RendererPopulateGraphicsPipelineCreateInfo
//------------------------------------------------------------------------------
SDL_bool
VSR_RendererPopulateGraphicsPipelineCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs)
{
	//////////////////////////
	/// Layout Create Info ///
	//////////////////////////
	VkPipelineLayoutCreateInfo* layoutCreateInfo =
		&subStructs->graphicsPipelineCreateInfo.graphicsPipelineLayoutCreateInfo;

	layoutCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo->pNext = NULL;
	layoutCreateInfo->flags = 0;


	////////////////////////////
	/// Pipeline Create Info ///
	////////////////////////////
	VkGraphicsPipelineCreateInfo* pipelineCreateInfo =
	 &subStructs->graphicsPipelineCreateInfo.graphicsPipelineCreateInfo;

	pipelineCreateInfo->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo->pNext = NULL;
	pipelineCreateInfo->flags = 0;

	SUCCESS:
	{
		return SDL_TRUE;
	}

	FAIL:
	{
		return SDL_FALSE;
	}
}
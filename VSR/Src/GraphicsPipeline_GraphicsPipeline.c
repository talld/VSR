#include "GraphicsPipeline_GraphicsPipeline.h"

#include "VSR_Renderer.h"
#include "VSR_error.h"
#include "VSR_Vertex.h"

#include "vert.h"
#include "frag.h"





//==============================================================================
// VSR_GraphicsPipelinePopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
GraphicsPipeline_GraphicsPipelinePopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo,
	GraphicsPipeline_CreateInfoSubStructs* subStructs)
{
	//////////////////////////
	/// Layout Create Info ///
	//////////////////////////
	VkPipelineLayoutCreateInfo* layoutCreateInfo =
		&subStructs->graphicsPipelineCreateInfo.graphicsPipelineLayoutCreateInfo;

	layoutCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo->pNext = NULL;
	layoutCreateInfo->flags = 0;

	/////////////////////
	/// Shader stages ///
	/////////////////////
	const char* shaderEntryPoint = "main";

	VkPipelineShaderStageCreateInfo* fragStageInfo =
		&subStructs->graphicsPipelineCreateInfo
			.shadersStages[SHADER_STAGE_FRAGMENT];

	fragStageInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageInfo->pNext = NULL;
	fragStageInfo->flags = 0L;
	fragStageInfo->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageInfo->pName = shaderEntryPoint;


	VkPipelineShaderStageCreateInfo* vertStageInfo =
		&subStructs->graphicsPipelineCreateInfo
			.shadersStages[SHADER_STAGE_VERTEX];

	vertStageInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageInfo->pNext = NULL;
	vertStageInfo->flags = 0L;
	vertStageInfo->stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageInfo->pName = shaderEntryPoint;

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
// VSR_GraphicsPipelineCreate
//------------------------------------------------------------------------------
SDL_bool
GraphicsPipeline_GraphicsPipelineCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	GraphicsPipeline_CreateInfoSubStructs* subStructs)
{
	///////////////
	/// aliases ///
	///////////////

	GraphicsPipeline_GraphicsPipeline* gp = &pipeline->subStructs->graphicPipeline;

	VkPipelineLayoutCreateInfo* layoutCreateInfo =
		&subStructs->graphicsPipelineCreateInfo.graphicsPipelineLayoutCreateInfo;

	VkPipelineShaderStageCreateInfo* shadersStages =
		subStructs->graphicsPipelineCreateInfo.shadersStages;

	VkGraphicsPipelineCreateInfo* pipelineCreateInfo =
		&subStructs->graphicsPipelineCreateInfo.graphicsPipelineCreateInfo;

	/////////////////////
	/// Shader stages ///
	/////////////////////

	if(pipeline->subStructs->fragmentShader)
	{
		shadersStages[SHADER_STAGE_FRAGMENT].module =
			pipeline->subStructs->fragmentShader->module;
	}

	VSR_Shader vertBackup =
		VSR_ShaderCreate(renderer, kVertexShaderBytecodeSize, kVertexShaderBytecode);
	shadersStages[SHADER_STAGE_VERTEX].module = vertBackup.module;

	VSR_Shader fragBackup =
		VSR_ShaderCreate(renderer, kFragmentShaderBytecodeSize, kFragmentShaderBytecode);
	shadersStages[SHADER_STAGE_FRAGMENT].module = fragBackup.module;

	if(pipeline->subStructs->vertexShader)
	{
		shadersStages[SHADER_STAGE_VERTEX].module =
			pipeline->subStructs->vertexShader->module;
	}

	if(pipeline->subStructs->fragmentShader)
	{
		shadersStages[SHADER_STAGE_VERTEX].module =
			pipeline->subStructs->fragmentShader->module;
	}


	////////////////////
	/// Vertex Input ///
	////////////////////

	VkVertexInputBindingDescription vertexInputDesc[1] = {0};

	vertexInputDesc[0].binding = 0;
	vertexInputDesc[0].stride = sizeof(VSR_Vertex);
	vertexInputDesc[0].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	VkVertexInputAttributeDescription vertexAttrDesc[1] = {0};
	vertexAttrDesc[0].binding = 0;
	vertexAttrDesc[0].location = 0;
	vertexAttrDesc[0].offset = offsetof(VSR_Vertex, x); // this may bite in ass;
	vertexAttrDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT; // vec3 nonsense...

	VkPipelineVertexInputStateCreateInfo vertInfo =
		(VkPipelineVertexInputStateCreateInfo){0};

	vertInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertInfo.pNext = NULL;
	vertInfo.flags = 0L;
	vertInfo.vertexBindingDescriptionCount = 0;
	vertInfo.pVertexBindingDescriptions = &vertexInputDesc[0];
	vertInfo.vertexAttributeDescriptionCount = 0;
	vertInfo.pVertexAttributeDescriptions = &vertexAttrDesc[0];

	//////////////////////
	/// Input Assembly ///
	//////////////////////

	VkPipelineInputAssemblyStateCreateInfo inputInfo =
		(VkPipelineInputAssemblyStateCreateInfo) {0};

	inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputInfo.pNext = NULL;
	inputInfo.flags = 0L;
	inputInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputInfo.primitiveRestartEnable = VK_FALSE;

	/////////////////
	/// View-Port ///
	/////////////////

	VkViewport viewport = (VkViewport){0};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = renderer->subStructs->surface.surfaceWidth;
	viewport.height = renderer->subStructs->surface.surfaceHeight;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	VkRect2D scissor = (VkRect2D){0};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = renderer->subStructs->surface.surfaceWidth;
	scissor.extent.height = renderer->subStructs->surface.surfaceHeight;

	VkPipelineViewportStateCreateInfo viewInfo =
		(VkPipelineViewportStateCreateInfo){0};

	viewInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewInfo.pNext = NULL;
	viewInfo.flags = 0L;
	viewInfo.viewportCount = 1;
	viewInfo.pViewports = &viewport;
	viewInfo.scissorCount = 1;
	viewInfo.pScissors = &scissor;

	/////////////////////
	/// Raster states ///
	/////////////////////

	VkPipelineRasterizationStateCreateInfo rasterInfo =
		(VkPipelineRasterizationStateCreateInfo){0};

	rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterInfo.pNext = NULL;
	rasterInfo.flags = 0L;
	rasterInfo.depthClampEnable = VK_FALSE;
	rasterInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterInfo.lineWidth = 1.f;
	rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterInfo.depthBiasEnable = VK_FALSE;

	//////////////////////
	/// Multi-Sampling ///
	//////////////////////

	VkPipelineMultisampleStateCreateInfo sampleInfo =
		(VkPipelineMultisampleStateCreateInfo){0};

	sampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	sampleInfo.pNext = NULL;
	sampleInfo.flags = 0L;
	sampleInfo.sampleShadingEnable = VK_FALSE;
	sampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	sampleInfo.alphaToCoverageEnable = VK_FALSE;
	sampleInfo.alphaToOneEnable = VK_FALSE;

	////////////////
	/// blending ///
	////////////////

	VkPipelineColorBlendAttachmentState colourInfo =
		(VkPipelineColorBlendAttachmentState){0};

	colourInfo.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;

	colourInfo.blendEnable = VK_TRUE;
	colourInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colourInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colourInfo.colorBlendOp = VK_BLEND_OP_ADD;
	colourInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colourInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colourInfo.colorBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo blendInfo =
		(VkPipelineColorBlendStateCreateInfo){0};

	blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendInfo.pNext = NULL;
	blendInfo.flags = 0L;
	blendInfo.logicOpEnable = VK_FALSE;
	blendInfo.logicOp = VK_LOGIC_OP_COPY;
	blendInfo.attachmentCount = 1;
	blendInfo.pAttachments = &colourInfo;

	/////////////////////
	/// Dynamic sates ///
	/////////////////////

	VkPipelineDynamicStateCreateInfo dynamicInfo =
		(VkPipelineDynamicStateCreateInfo){0};

	dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicInfo.pNext = NULL;
	dynamicInfo.flags = 0L;
	dynamicInfo.dynamicStateCount = 0;
	dynamicInfo.pDynamicStates = NULL;

	//////////////
	/// layout ///
	//////////////

	layoutCreateInfo->setLayoutCount = 0;
	layoutCreateInfo->pSetLayouts = NULL;
	layoutCreateInfo->pushConstantRangeCount = 0;
	layoutCreateInfo->pPushConstantRanges = NULL;

	VkResult err =
		vkCreatePipelineLayout(renderer->subStructs->logicalDevice.device,
							   layoutCreateInfo,
							   VSR_GetAllocator(),
							   &gp->pipelineLayout);

	if(err != VK_SUCCESS)
	{
		char errMsg[255];
		sprintf(errMsg, "Failed to create pipeline layout: %s",
				VSR_VkErrorToString(err));

		VSR_SetErr(errMsg);
		goto FAIL;
	}

	////////////////////////////
	/// Pipeline Create Info ///
	////////////////////////////
	pipelineCreateInfo->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo->pNext = NULL;
	pipelineCreateInfo->flags = 0;
	pipelineCreateInfo->stageCount = 2;
	pipelineCreateInfo->pStages = subStructs->graphicsPipelineCreateInfo.shadersStages;
	pipelineCreateInfo->pVertexInputState = &vertInfo;
	pipelineCreateInfo->pInputAssemblyState = &inputInfo;
	pipelineCreateInfo->pViewportState = &viewInfo;
	pipelineCreateInfo->pRasterizationState = &rasterInfo;
	pipelineCreateInfo->pMultisampleState = &sampleInfo;
	pipelineCreateInfo->pColorBlendState = &blendInfo;
	pipelineCreateInfo->pDynamicState = &dynamicInfo;
	pipelineCreateInfo->basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo->basePipelineIndex = 0;
	pipelineCreateInfo->layout = gp->pipelineLayout;
	pipelineCreateInfo->renderPass = pipeline->subStructs->renderPass.renderPass;
	pipelineCreateInfo->subpass = 0;

	vkCreateGraphicsPipelines(renderer->subStructs->logicalDevice.device,
							  NULL,
							  1,
							  pipelineCreateInfo,
							  VSR_GetAllocator(),
							  &gp->pipeline);

	// backup shaders are managed by us so delete them here
	VSR_ShaderDestroy(renderer, &fragBackup);
	VSR_ShaderDestroy(renderer, &vertBackup);
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
// VSR_GraphicsPipelinePopulateCreateInfo
//------------------------------------------------------------------------------
void
GraphicsPipeline_GraphicPipelineDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline
)
{
	vkDestroyPipeline(renderer->subStructs->logicalDevice.device,
					  pipeline->subStructs->graphicPipeline.pipeline,
					  VSR_GetAllocator());

	vkDestroyPipelineLayout(renderer->subStructs->logicalDevice.device,
							pipeline->subStructs->graphicPipeline.pipelineLayout,
							VSR_GetAllocator());
}
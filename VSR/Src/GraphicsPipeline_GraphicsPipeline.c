#include "GraphicsPipeline_GraphicsPipeline.h"

#include "VSR_Renderer.h"
#include "VSR_error.h"

#include "VSR_VertexShaderDefault.h"
#include "VSR_FragmentShaderDefault.h"





//==============================================================================
// VSR_GraphicsPipelinePopulateCreateInfo
//------------------------------------------------------------------------------
SDL_bool
GraphicsPipeline_GraphicsPipelinePopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo)
{
	//////////////////////////
	/// Layout Create Info ///
	//////////////////////////
	VkPipelineLayoutCreateInfo* layoutCreateInfo =
		&createInfo->subStructs->graphicsPipelineCreateInfo.graphicsPipelineLayoutCreateInfo;

	layoutCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo->pNext = NULL;
	layoutCreateInfo->flags = 0;

	/////////////////////
	/// Shader stages ///
	/////////////////////
	const char* shaderEntryPoint = "main";

	VkPipelineShaderStageCreateInfo* fragStageInfo =
		&createInfo->subStructs->graphicsPipelineCreateInfo
			.shadersStages[SHADER_STAGE_FRAGMENT];

	fragStageInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageInfo->pNext = NULL;
	fragStageInfo->flags = 0L;
	fragStageInfo->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageInfo->pName = shaderEntryPoint;


	VkPipelineShaderStageCreateInfo* vertStageInfo =
		&createInfo->subStructs->graphicsPipelineCreateInfo
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
	VSR_GraphicsPipelineCreateInfo* createInfo)
{
	///////////////
	/// aliases ///
	///////////////

	GraphicsPipeline_GraphicsPipeline* gp = &pipeline->subStructs->graphicPipeline;

	VkPipelineLayoutCreateInfo* layoutCreateInfo =
		&createInfo->subStructs->graphicsPipelineCreateInfo.graphicsPipelineLayoutCreateInfo;

	VkPipelineShaderStageCreateInfo* shadersStages =
		createInfo->subStructs->graphicsPipelineCreateInfo.shadersStages;

	VkGraphicsPipelineCreateInfo* pipelineCreateInfo =
		&createInfo->subStructs->graphicsPipelineCreateInfo.graphicsPipelineCreateInfo;

	/////////////////////
	/// Shader stages ///
	/////////////////////
	VSR_Shader* vertBackup =
		VSR_ShaderCreate(renderer, kVertexShaderBytecodeSize, kVertexShaderByteCode);
	shadersStages[SHADER_STAGE_VERTEX].module = vertBackup->module;

	VSR_Shader* fragBackup =
		VSR_ShaderCreate(renderer, kFragmentShaderBytecodeSize, kFragmentShaderByteCode);
	shadersStages[SHADER_STAGE_FRAGMENT].module = fragBackup->module;

	if(createInfo->vertexShader)
	{
		shadersStages[SHADER_STAGE_VERTEX].module =
			createInfo->vertexShader->module;
	}

	if(createInfo->fragmentShader)
	{
		shadersStages[SHADER_STAGE_FRAGMENT].module =
			createInfo->fragmentShader->module;
	}

	////////////////////
	/// Vertex Input ///
	////////////////////

	// VSR_VERTEX          vec3
	// VSR_UV              vec2
	// VSR_TRANSFORM       vec4[0]
	// VSR_TRANSFORM       vec4[1]
	// VSR_TRANSFORM       vec4[2]
	// VSR_TRANSFORM       vec4[3]
	// VSR_SAMPLER (index) int32

	enum {
		kVertexInput = 0,
		kNormalInput = 1,
		kUVInput = 2,
		kSamplerInput = 3,
		kMat4Input = 4,
		kVertexInputCount = 5};


	enum {
		kVertexBinding = 0,
		kNormalBinding = 1,
		kUVBinding = 2,
		kSamplerBiding = 3,
		kMat4Row1Biding = 4,
		kMat4Row2Biding = 5,
		kMat4Row3Biding = 6,
		kMat4Row4Biding = 7,
		kVertexBindingCount = 8};

	VkVertexInputBindingDescription vertexInputDesc[kVertexInputCount] = {0};
	VkVertexInputAttributeDescription vertexAttrDesc[kVertexBindingCount] = {0};



	// vertices [0]
	vertexInputDesc[kVertexInput].binding = kVertexInput;
	vertexInputDesc[kVertexInput].stride = sizeof(VSR_Vertex);
	vertexInputDesc[kVertexInput].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	vertexAttrDesc[kVertexBinding].binding = kVertexBinding;
	vertexAttrDesc[kVertexBinding].location = kVertexInput;
	vertexAttrDesc[kVertexBinding].offset = 0;
	vertexAttrDesc[kVertexBinding].format = VK_FORMAT_R32G32B32_SFLOAT;

	// normals [1]
	vertexInputDesc[kNormalInput].binding = kNormalInput;
	vertexInputDesc[kNormalInput].stride = sizeof(VSR_Vertex);
	vertexInputDesc[kNormalInput].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	vertexAttrDesc[kNormalBinding].location = kNormalInput;
	vertexAttrDesc[kNormalBinding].binding = kNormalBinding;
	vertexAttrDesc[kNormalBinding].offset = 0;
	vertexAttrDesc[kNormalBinding].format = VK_FORMAT_R32G32B32_SFLOAT;

	// UVs [1]
	vertexInputDesc[kUVInput].binding = kUVInput;
	vertexInputDesc[kUVInput].stride = sizeof(VSR_UV);
	vertexInputDesc[kUVInput].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	vertexAttrDesc[kUVBinding].binding = kUVInput;
	vertexAttrDesc[kUVBinding].location = kUVBinding;
	vertexAttrDesc[kUVBinding].offset = 0;
	vertexAttrDesc[kUVBinding].format = VK_FORMAT_R32G32_SFLOAT; // vec2

	// sampler [2]
	vertexInputDesc[kSamplerInput].binding = kSamplerInput;
	vertexInputDesc[kSamplerInput].stride = sizeof(uint32_t);
	vertexInputDesc[kSamplerInput].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	vertexAttrDesc[kSamplerBiding].binding = kSamplerInput;
	vertexAttrDesc[kSamplerBiding].location = kSamplerBiding;
	vertexAttrDesc[kSamplerBiding].offset = 0;
	vertexAttrDesc[kSamplerBiding].format = VK_FORMAT_R32_UINT; // int32

	// transform
	// mat4 is is actually vec4[4]..
	// so uses 4 bindings..

	// mat4 row1 [3]
	vertexInputDesc[kMat4Input].binding = kMat4Input;
	vertexInputDesc[kMat4Input].stride = sizeof(VSR_Mat4);
	vertexInputDesc[kMat4Input].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	vertexAttrDesc[kMat4Row1Biding].binding = kMat4Input;
	vertexAttrDesc[kMat4Row1Biding].location = kMat4Row1Biding;
	vertexAttrDesc[kMat4Row1Biding].offset =  sizeof(float[4]) * 0;
	vertexAttrDesc[kMat4Row1Biding].format = VK_FORMAT_R32G32B32A32_SFLOAT; // vec4

	// mat4 row2 [4]
	vertexAttrDesc[kMat4Row2Biding].binding = kMat4Input;
	vertexAttrDesc[kMat4Row2Biding].location = kMat4Row2Biding;
	vertexAttrDesc[kMat4Row2Biding].offset =  sizeof(float[4]) * 1;
	vertexAttrDesc[kMat4Row2Biding].format = VK_FORMAT_R32G32B32A32_SFLOAT; // vec4

	// mat4 row3 [5]
	vertexAttrDesc[kMat4Row3Biding].binding = kMat4Input;
	vertexAttrDesc[kMat4Row3Biding].location = kMat4Row3Biding;
	vertexAttrDesc[kMat4Row3Biding].offset =  sizeof(float[4]) * 2;
	vertexAttrDesc[kMat4Row3Biding].format = VK_FORMAT_R32G32B32A32_SFLOAT; // vec4

	// mat4 row4 [6]
	vertexAttrDesc[kMat4Row4Biding].binding = kMat4Input;
	vertexAttrDesc[kMat4Row4Biding].location = kMat4Row4Biding;
	vertexAttrDesc[kMat4Row4Biding].offset =  sizeof(float[4]) * 3;
	vertexAttrDesc[kMat4Row4Biding].format = VK_FORMAT_R32G32B32A32_SFLOAT; // vec4

	VkPipelineVertexInputStateCreateInfo vertInfo =
		(VkPipelineVertexInputStateCreateInfo){0};

	vertInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertInfo.pNext = NULL;
	vertInfo.flags = 0L;
	vertInfo.vertexBindingDescriptionCount = kVertexInputCount;
	vertInfo.pVertexBindingDescriptions = vertexInputDesc;
	vertInfo.vertexAttributeDescriptionCount = kVertexBindingCount;
	vertInfo.pVertexAttributeDescriptions = vertexAttrDesc ;

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
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

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
	rasterInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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
	/// depth stencil ///
	/////////////////////
	VkPipelineDepthStencilStateCreateInfo stencilCreateInfo
		= (VkPipelineDepthStencilStateCreateInfo){0};

	stencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	stencilCreateInfo.pNext = NULL;
	stencilCreateInfo.flags = 0L;
	stencilCreateInfo.depthTestEnable = VK_TRUE;
	stencilCreateInfo.depthWriteEnable = VK_TRUE;
	stencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	stencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	stencilCreateInfo.stencilTestEnable = VK_FALSE;

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

	//////////////////////
	/// push constants ///
	//////////////////////
	enum { pushConstantCount = 2 };
	VkPushConstantRange pushConstants[pushConstantCount];
	pushConstants[0].offset = 0;
	pushConstants[0].size = sizeof(VSR_PushConstants);
	pushConstants[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	pushConstants[1].offset =  sizeof(VSR_PushConstants);
	pushConstants[1].size = sizeof(VSR_PushConstants);
	pushConstants[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	//////////////
	/// layout ///
	//////////////

	enum {kLayoutCount = 2};
	VkDescriptorSetLayout layouts[kLayoutCount] = {
		renderer->subStructs->descriptorPool.globalLayout,
		renderer->subStructs->descriptorPool.userLayout
	};

	layoutCreateInfo->setLayoutCount = 1 + (renderer->subStructs->extraDescriptorCount > 0);
	layoutCreateInfo->pSetLayouts = layouts;

	layoutCreateInfo->pushConstantRangeCount = pushConstantCount;
	layoutCreateInfo->pPushConstantRanges = pushConstants;

	VkResult err =
		vkCreatePipelineLayout(renderer->subStructs->logicalDevice.device,
							   layoutCreateInfo,
							   VSR_GetAllocator(),
							   &gp->pipelineLayout);

	if(err != VK_SUCCESS)
	{
        VSR_Error("Failed to create pipeline layout: %s",
				VSR_VkErrorToString(err));
        goto FAIL;
	}

	////////////////////////////
	/// Pipeline Create Info ///
	////////////////////////////
	pipelineCreateInfo->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo->pNext = NULL;
	pipelineCreateInfo->flags = 0;
	pipelineCreateInfo->stageCount = 2;
	pipelineCreateInfo->pStages = createInfo->subStructs->graphicsPipelineCreateInfo.shadersStages;
	pipelineCreateInfo->pVertexInputState = &vertInfo;
	pipelineCreateInfo->pInputAssemblyState = &inputInfo;
	pipelineCreateInfo->pViewportState = &viewInfo;
	pipelineCreateInfo->pRasterizationState = &rasterInfo;
	pipelineCreateInfo->pMultisampleState = &sampleInfo;
	pipelineCreateInfo->pColorBlendState = &blendInfo;
	pipelineCreateInfo->pDepthStencilState = &stencilCreateInfo;
	pipelineCreateInfo->pDynamicState = &dynamicInfo;
	pipelineCreateInfo->pDynamicState = &dynamicInfo;
	pipelineCreateInfo->basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo->basePipelineIndex = 0;
	pipelineCreateInfo->renderPass = pipeline->subStructs->renderPass.renderPass;
	pipelineCreateInfo->subpass = 0;
	pipelineCreateInfo->layout = gp->pipelineLayout;


	vkCreateGraphicsPipelines(renderer->subStructs->logicalDevice.device,
							  NULL,
							  1,
							  pipelineCreateInfo,
							  VSR_GetAllocator(),
							  &gp->pipeline);

	// backup shaders are managed by us so delete them here
	VSR_ShaderDestroy(renderer, fragBackup);
	VSR_ShaderDestroy(renderer, vertBackup);
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
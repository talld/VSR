#ifndef VSR_SUITE_RENDERER_GRAPHICSPIPELINE_H
#define VSR_SUITE_RENDERER_GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>

typedef struct VSR_GraphicPipelineCreateInfo VSR_GraphicPipelineCreateInfo;
struct VSR_GraphicPipelineCreateInfo
{
	VkPipelineLayoutCreateInfo   graphicsPipelineLayoutCreateInfo;
	VkPipelineShaderStageCreateInfo shadersStages[2];
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
};


typedef struct VSR_GraphicPipeline VSR_GraphicPipeline;
struct VSR_GraphicPipeline
{
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
};


SDL_bool
VSR_GraphicsPipelinePopulateCreateInfo(
	VSR_RendererCreateInfo* createInfo,
	VSR_RendererCreateInfoSubStructs* subStructs);


SDL_bool
VSR_GraphicsPipelineCreate(
	VSR_Renderer* renderer,
	VSR_RendererCreateInfoSubStructs* subStructs);

void
VSR_GraphicPipelineDestroy(
	VSR_Renderer* renderer
	);

#endif //VSR_SUITE_RENDERER_GRAPHICSPIPELINE_H

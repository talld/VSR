#ifndef VSR_SUITE_RENDERER_GRAPHICSPIPELINE_H
#define VSR_SUITE_RENDERER_GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>


typedef struct VSR_GraphicPipelineCreateInfo VSR_GraphicPipelineCreateInfo;
struct VSR_GraphicPipelineCreateInfo
{
	VkPipelineLayoutCreateInfo   graphicsPipelineLayoutCreateInfo;
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
};

#endif //VSR_SUITE_RENDERER_GRAPHICSPIPELINE_H

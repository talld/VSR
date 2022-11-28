#ifndef VSR_GRAPHICSPIPELINE_H
#define VSR_GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>


////////////////////////////////
/// Renderer pre-declaration ///
////////////////////////////////
typedef struct Renderer_CreateInfoSubStructs Renderer_CreateInfoSubStructs;
struct Renderer_CreateInfoSubStructs;

////////////////////////////////////////
/// GraphicsPipeline pre-declaration ///
////////////////////////////////////////
typedef struct GraphicsPipeline_CreateInfoSubStructs GraphicsPipeline_CreateInfoSubStructs;
struct GraphicsPipeline_CreateInfoSubStructs;
typedef struct VSR_GraphicsPipelineCreateInfo VSR_GraphicsPipelineCreateInfo;
struct VSR_GraphicsPipelineCreateInfo;


typedef struct GraphicsPipeline_GraphicsPipelineCreateInfo GraphicsPipeline_GraphicsPipelineCreateInfo;
struct GraphicsPipeline_GraphicsPipelineCreateInfo
{
	VkPipelineLayoutCreateInfo   graphicsPipelineLayoutCreateInfo;
	VkPipelineShaderStageCreateInfo shadersStages[2];
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
};


typedef struct GraphicsPipeline_GraphicsPipeline GraphicsPipeline_GraphicsPipeline;
struct GraphicsPipeline_GraphicsPipeline
{
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
};


SDL_bool
GraphicsPipeline_GraphicsPipelinePopulateCreateInfo(
	VSR_Renderer* renderer,
	VSR_GraphicsPipelineCreateInfo* createInfo,
	GraphicsPipeline_CreateInfoSubStructs* subStructs);


SDL_bool
GraphicsPipeline_GraphicsPipelineCreate(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline,
	GraphicsPipeline_CreateInfoSubStructs* subStructs);

void
GraphicsPipeline_GraphicPipelineDestroy(
	VSR_Renderer* renderer,
	VSR_GraphicsPipeline* pipeline
	);

#endif //VSR_GRAPHICSPIPELINE_H

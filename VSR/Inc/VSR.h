#ifndef VSR_H
#define VSR_H

////////////////////////////////////////////////////////////////////////////////
/// Library includes                                                         ///
////////////////////////////////////////////////////////////////////////////////

// C std-lib
#include <stddef.h>
#include <stdint.h>

// SDL
#include <SDL2/SDL.h>

#define VSR_DEBUG 1

////////////////////////////////////////////////////////////////////////////////
/// Struct pre-declarations                                                  ///
////////////////////////////////////////////////////////////////////////////////
typedef struct Renderer_ModelBuffer Renderer_ModelBuffer;
struct Renderer_ModelBuffer;

typedef struct VSR_RendererCreateInfo VSR_RendererCreateInfo;
struct VSR_RendererCreateInfo;

typedef struct VSR_Renderer VSR_Renderer;
struct VSR_Renderer;

typedef struct VSR_GraphicsPipelineCreateInfo VSR_GraphicsPipelineCreateInfo;
struct VSR_GraphicsPipelineCreateInfo;

typedef struct VSR_GraphicsPipeline VSR_GraphicsPipeline;
struct VSR_GraphicsPipeline;

typedef enum VSR_ShaderStage VSR_ShaderStage;
enum VSR_ShaderStage
{
	SHADER_STAGE_FRAGMENT = 0,
	SHADER_STAGE_VERTEX = 1,
};

typedef struct VSR_Shader VSR_Shader;
struct VSR_Shader;

typedef struct VSR_Mat4 VSR_Mat4;
__attribute__((packed))
struct VSR_Mat4
{
	float m0;
	float m1;
	float m2;
	float m3;
	float m4;
	float m5;
	float m6;
	float m7;
	float m8;
	float m9;
	float m10;
	float m11;
	float m12;
	float m13;
	float m14;
	float m15;
};

typedef struct VSR_PushConstants VSR_PushConstants;
struct VSR_PushConstants
{
	VSR_Mat4 Projection;
	uint8_t bytes[64];
};

typedef struct VSR_Vertex VSR_Vertex;
__attribute__((packed))
struct VSR_Vertex
{
	float x;
	float y;
	float z;
};

typedef struct VSR_Index VSR_Index;
struct VSR_Index
{
	uint32_t i;
};

typedef struct VSR_UV VSR_UV;
struct VSR_UV
{
	float x;
	float y;
};

typedef struct VSR_Mesh VSR_Mesh;
struct VSR_Mesh
{
	size_t      vertexCount;
	VSR_Vertex* vertices;
	VSR_Vertex* normals;
	VSR_UV*     UVs;

	size_t      indexCount;
	VSR_Index*  indices;
};

typedef struct VSR_Model VSR_Model;
struct VSR_Model;

typedef struct VSR_Sampler VSR_Sampler;
struct VSR_Sampler;

////////////////////////////////////////////////////////////////////////////////
/// Enum pre-declarations                                                    ///
////////////////////////////////////////////////////////////////////////////////
typedef enum VSR_ImageFormat VSR_ImageFormat;
enum VSR_ImageFormat;

typedef enum VSR_SamplerFlags VSR_SamplerFlags;
enum VSR_SamplerFlags;

typedef enum VSR_CreateInfoFlags VSR_CreateInfoFlags;
enum VSR_CreateInfoFlags
{
	VSR_CREATE_INFO_NO_FLAGS = 0,
	VSR_CREATE_INFO_GEOMETRY_SHADER             = 0x1,
	VSR_CREATE_INFO_TESSELATION_SHADER          = 0x2,
	VSR_CREATE_INFO_COMPUTE_SHADER              = 0x4,
	VSR_CREATE_INFO_TRIANGLES_COUNTER_CLOCKWISE = 0x8,

};


////////////////////////////////////////////////////////////////////////////////
/// Function definitions                                                     ///
////////////////////////////////////////////////////////////////////////////////

// general
const char* VSR_getErr();

// Renderer
VSR_RendererCreateInfo* VSR_RendererGenerateCreateInfo(SDL_Window* window);

VSR_RendererCreateInfo* VSR_RendererRequestTextureCount(VSR_RendererCreateInfo* createInfo, size_t count);

VSR_RendererCreateInfo* VSR_RendererRequestDescriptor(VSR_RendererCreateInfo* createInfo, size_t index, size_t size);

void VSR_RendererFreeCreateInfo(VSR_RendererCreateInfo* rendererCreateInfo);

VSR_Renderer* VSR_RendererCreate(VSR_RendererCreateInfo* rendererCreateInfo);

void VSR_RendererFree(VSR_Renderer* renderer);

void VSR_RendererSetPipeline(VSR_Renderer* renderer, VSR_GraphicsPipeline* pipeline);

void VSR_RendererBeginPass(VSR_Renderer* renderer);

void VSR_RendererEndPass(VSR_Renderer* renderer);

void VSR_RendererSetVertexConstants(VSR_Renderer* renderer, VSR_PushConstants const* pushConstants);

void VSR_RendererSetFragmentConstants(VSR_Renderer* renderer, VSR_PushConstants const* pushConstants);

void VSR_RendererWriteDescriptor(VSR_Renderer* renderer, size_t index, size_t offset, void* data, size_t len);

VSR_GraphicsPipelineCreateInfo* VSR_GraphicsPipelineGenerateCreateInfo(VSR_Renderer* renderer);

void VSR_GraphicsPipelineCreateInfoFree(VSR_GraphicsPipelineCreateInfo* createInfo);

VSR_GraphicsPipeline* VSR_GraphicsPipelineCreate(VSR_Renderer* renderer, VSR_GraphicsPipelineCreateInfo* createInfo);

void VSR_GraphicsPipelineFree(VSR_Renderer* renderer, VSR_GraphicsPipeline* pipeline);

void VSR_GraphicsPipelineSetShader(VSR_GraphicsPipelineCreateInfo* pipeline, VSR_ShaderStage stage, VSR_Shader* shader);

VSR_Shader* VSR_ShaderCreate(VSR_Renderer* renderer, size_t byteCount, const uint8_t* bytes);

void VSR_ShaderDestroy(VSR_Renderer* renderer, VSR_Shader* shader);

void VSR_RendererSetShader(VSR_Renderer* renderer, VSR_ShaderStage stage, VSR_Shader* shader);

int VSR_RenderModels(VSR_Renderer* renderer, VSR_Model* models, VSR_Mat4* transforms, VSR_Sampler** samplers, size_t batchCount);

VSR_Mesh* VSR_MeshCreate(size_t vertexCount, VSR_Vertex const* vertices, VSR_Vertex const* normals, VSR_UV const* UVs, size_t indexCount, VSR_Index const* indices);

void VSR_MeshFree(VSR_Mesh* model);

VSR_Model* VSR_ModelCreate(VSR_Renderer* renderer, VSR_Mesh* mesh);

void VSR_ModelFree(VSR_Renderer* renderer, VSR_Model* model);

void VSR_ModelUpdate(VSR_Renderer* renderer, VSR_Model* model);

VSR_Sampler* VSR_SamplerCreate(VSR_Renderer* renderer, VSR_GraphicsPipeline* pipeline, size_t index, SDL_Surface* sur);

int VSR_SetSampler(VSR_Renderer* renderer, VSR_Sampler* sampler);

#endif // VSR_H

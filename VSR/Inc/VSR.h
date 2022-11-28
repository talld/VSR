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
typedef struct VSR_RendererCreateInfoSubStructs VSR_RendererCreateInfoSubStructs;
struct VSR_RendererCreateInfoSubStructs;

typedef struct VSR_RendererCreateInfo VSR_RendererCreateInfo;
struct VSR_RendererCreateInfo;

typedef struct VSR_Renderer VSR_Renderer;
struct VSR_Renderer;

typedef enum VSR_ShaderStage VSR_ShaderStage;
enum VSR_ShaderStage;

typedef struct VSR_Shader VSR_Shader;
struct VSR_Shader;

typedef struct VSR_Model VSR_Model;
struct VSR_Model;

typedef struct VSR_Sampler VSR_Sampler;
struct VSR_Sampler;

typedef struct VSR_Transform VSR_Transform;
struct VSR_Transform;


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
VSR_RendererCreateInfo* VSR_RendererGenerateCreateInfo(SDL_Window* window, VSR_CreateInfoFlags flags);

void VSR_RendererFreeCreateInfo(VSR_RendererCreateInfo* rendererCreateInfo);

VSR_Renderer* VSR_RendererCreate(VSR_RendererCreateInfo* rendererCreateInfo);

void VSR_RendererFree(VSR_Renderer* renderer);

void VSR_RendererBeginPass(VSR_Renderer* renderer);

void VSR_RendererEndPass(VSR_Renderer* renderer);

VSR_Shader VSR_ShaderCreate(VSR_Renderer* renderer, size_t byteCount, const uint8_t* bytes);

void VSR_ShaderDestroy(VSR_Renderer* renderer, VSR_Shader* shader);

void VSR_RendererSetShader(VSR_Renderer* renderer, VSR_ShaderStage stage, VSR_Shader* shader);

int VSR_RenderModels(VSR_Renderer* renderer, VSR_Model* models, VSR_Transform* transforms, size_t batchCount);

VSR_Model* VSR_CreateModel(VSR_Renderer* renderer, int32_t* vertices, size_t vertiexCount, int32_t* indices, size_t indexCount);

int VSR_BindUVs(VSR_Renderer* renderer, VSR_Model* model, int32_t* UVs);

VSR_Sampler* VSR_CreateSampler(SDL_Surface* image, VSR_ImageFormat format, VSR_SamplerFlags flags);

VSR_Transform* VSR_CreateTransforms(size_t count);

void VSR_FreeTransforms(size_t count);

VSR_Transform VSR_SetTransformsPosition(VSR_Transform transform, float x, float y, float z);

VSR_Transform VSR_SetTransformsRotation(VSR_Transform transform, float xRot, float yRot, float zRot);

VSR_Transform VSR_SetTransformsScale(VSR_Transform transform, float xScale, float yScale, float zScale);

int VSR_SetSampler(VSR_Renderer* renderer, VSR_Sampler* sampler);

#endif // VSR_H

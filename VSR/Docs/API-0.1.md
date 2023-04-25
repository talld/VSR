```c
enum VSR_SamplerFlags
{
  SAMPLER_FLAG_RENDER_TARGET = 0x1
};
```

```c
const char* VSR_getErr();
```

```c
VSR_RendererCreateInfo* VSR_RendererGenerateCreateInfo(SDL_Window* window);
```

```c
VSR_RendererCreateInfo* VSR_RendererRequestTextureCount(VSR_RendererCreateInfo* createInfo, size_t count);
```

```c
VSR_RendererCreateInfo* VSR_RendererRequestDescriptor(VSR_RendererCreateInfo* createInfo, size_t index, size_t size);
```

```c
void VSR_RendererFreeCreateInfo(VSR_RendererCreateInfo* rendererCreateInfo);
```

```c
VSR_Renderer* VSR_RendererCreate(VSR_RendererCreateInfo* rendererCreateInfo);
```

```c
void VSR_RendererFree(VSR_Renderer* renderer);
```

```c
void VSR_RendererSetPipeline(VSR_Renderer* renderer, VSR_GraphicsPipeline* pipeline);
```

```c
void VSR_RendererBeginPass(VSR_Renderer* renderer);
```

```c
int Renderer_FlushQueuedModels(VSR_Renderer* renderer);
```

```c
void VSR_RendererEndPass(VSR_Renderer* renderer);
```

```c
int VSR_RendererSetRenderTarget(VSR_Renderer* renderer, VSR_Sampler* sampler);
```

```c
void VSR_RendererWriteDescriptor(VSR_Renderer* renderer, size_t index, size_t offset, void* data, size_t len);
```

```c
int VSR_RenderModels(VSR_Renderer* renderer, VSR_Model* models, VSR_Mat4** transforms, VSR_Sampler** samplers, size_t batchCount);
```

```c
VSR_GraphicsPipelineCreateInfo* VSR_GraphicsPipelineGenerateCreateInfo(VSR_Renderer* renderer);
```

```c
void VSR_GraphicsPipelineCreateInfoFree(VSR_GraphicsPipelineCreateInfo* createInfo);
```

```c
VSR_GraphicsPipeline* VSR_GraphicsPipelineCreate(VSR_Renderer* renderer, VSR_GraphicsPipelineCreateInfo* createInfo);
```

```c
void VSR_GraphicsPipelineFree(VSR_Renderer* renderer, VSR_GraphicsPipeline* pipeline);
```

```c
void VSR_GraphicsPipelineSetShader(VSR_GraphicsPipelineCreateInfo* pipeline, VSR_ShaderStage stage, VSR_Shader* shader);
```

```c
enum VSR_ShaderStage
{
  SHADER_STAGE_VERTEX = 0,
  SHADER_STAGE_FRAGMENT = 1,
};
```


```c
void VSR_PipelineSetPushConstants(VSR_GraphicsPipeline* pipeline, VSR_PushConstants const* pushConstants);
```

```c
struct VSR_PushConstants
{
  VSR_Mat4* Projection;
  uint8_t* bytes;
};
```

```c
VSR_Shader* VSR_ShaderCreate(VSR_Renderer* renderer, size_t byteCount, const uint8_t* bytes);
```

```c
void VSR_ShaderDestroy(VSR_Renderer* renderer, VSR_Shader* shader);
```

# Meshes and models

## Meshes
```c
VSR_Mesh* VSR_MeshCreate(size_t vertexCount, VSR_Vertex const* vertices, VSR_Vertex const* normals, VSR_UV const* UVs, size_t indexCount, VSR_Index const* indices);
```
meshes are created on in system memory and represent the resources required to make a model
vertexCount must be equal to the size of the arrays passed to vertices and a multiple of 3
vertex can have normal and UVs attached to them, any pointer of not NULL passed through will assume that every vertex has the associated attribute at the relevant index
vertex can also be modified by indices, these again must be a multiple of 3.
```c
void VSR_MeshFree(VSR_Mesh* model);
```
just as meshes are created they must be destroyed, mesh can be destroyed directly after the associated model created if they are no longer required
```c
VSR_PACKED(struct VSR_Mesh)
{
  size_t      vertexCount;
  VSR_Vertex* vertices;
  VSR_Vertex* normals;
  VSR_UV*     UVs;

  size_t      indexCount;
  VSR_Index*  indices;
};
```
As a meshes content is only important at time of model creation, they may be freely edited, however the rules for mesh creation still apply
```c
VSR_PACKED(struct VSR_Vertex)
{
  float x;
  float y;
  float z;
};
```

```c
VSR_PACKED(struct VSR_Index)
{
  uint32_t i;
};
```

```c
VSR_PACKED(struct VSR_UV)
{
  float x;
  float y;
};
```

```c
VSR_Model* VSR_ModelCreate(VSR_Renderer* renderer, VSR_Mesh* mesh);
```

```c
void VSR_ModelFree(VSR_Renderer* renderer, VSR_Model* model);
```

```c
void VSR_ModelUpdate(VSR_Renderer* renderer, VSR_Model* model);
```

```c
VSR_Sampler* VSR_SamplerCreate(VSR_Renderer* renderer, size_t index, SDL_Surface* sur, VSR_SamplerFlags flags);
```

```c
void VSR_SamplerFree(VSR_Renderer* renderer, VSR_Sampler* sampler);
```

```c
int VSR_SetSampler(VSR_Renderer* renderer, VSR_Sampler* sampler);
```

```c
VSR_Mat4* VSR_Mat4Create(VSR_Renderer* renderer, float* m);
```

```c
VSR_Mat4* VSR_Mat4Update(VSR_Renderer* renderer, VSR_Mat4* mat4, float* m);
```

```c
void VSR_Mat4Destroy(VSR_Renderer* renderer, VSR_Mat4* mat4);
```
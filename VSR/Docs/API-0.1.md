
# Models
```c
VSR_Model* VSR_CreateModel(VSR_Renderer* renderer, int32_t* vertices, size_t vertiexCount, int32_t* indices, size_t indexCount);
```
models are created as a list of vertices and indices points
vertices having a stride of 3, being: x y z points with verticesCount being a multiple of 3 and the number of these xyz groups.
Ensure that your vertices are in the correct order as your triangle winding ( default is counter clockwise )
```c
int VSR_BindUVs(VSR_Renderer* renderer, VSR_Model* model, int32_t* UVs);
```
UVs are bound optionally and like vertices are in a u v ground, 1 uv is required per vertex so UVCount === vertexCount at all times, as a result this function does not take a UVCount instead vertexCount will be attempted to be read from the UVs pointer

# Samplers
```c
VSR_Sampler* VSR_CreateSampler(SDL_Surface* image, VSR_ImageFormat format, VSR_SamplerFlags flags);
```
create a image sampler for the corresponding image to be used during a model render when a model with appropriate UVs is drawn

# Transforms
```c
VSR_Transform* VSR_CreateTransforms(size_t count);
```
transforms are created

```c
void VSR_FreeTransforms(size_t count);
```
transforms are freed

```c
VSR_Transform VSR_SetTransformsPosition(VSR_Transform transform, float x, float y, float z);
```
```c
VSR_Transform VSR_SetTransformsRotation(VSR_Transform transform, float xRot, float yRot, float zRot);
```
```c
VSR_Transform VSR_SetTransformsScale(VSR_Transform transform, float xScale, float yScale, float zScale);
```
used to set attributes of a transform

# Renderer
```c
VSR_RendererCreateInfo* VSR_RendererGenerateCreateInfo(VSR_CreateInfoFlags flags);
```
VSR_RendererCreateInfo is an amalgam of all the possible alterable settings that VSR supports
for example triangle winding can be set with
```c
// settings groupings not final
	VSR_RendererCreateInfo* cInfo = VSR_RendererGenerateCreateInfo(NULL);
	cInfo->polygonInfo->winding = VSR_CREATE_INFO_CLOCKWISE_WINDING
```
once your create info has been configured you can create the renderer with:
 ```c
VSR_Renderer* VSR_CreateRenderer(VSR_RendererCreateInfo* rendererCreateInfo);
```
Creates a VSR Render which holds all the state and vulkan settings
```c
int VSR_SetSampler(VSR_Renderer* renderer, VSR_Sampler* sampler);
```
Set the sample stage that appropriate model UVs will sample when rendered
```c
int VSR_RenderModels(VSR_Renderer* renderer, VSR_Model* models, VSR_Transform* transforms, size_t batchCount);
```
sends a list of models and transforms to the renderer of count batchCount allowing models to easily batched for efficient draw-calls in multi-model scenes
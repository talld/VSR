#include "VSR_Sampler.h"

#include "VSR_Renderer.h"
#include "VSR_Framebuffer.h"
#include "VSR_error.h"


VSR_Sampler*
VSR_SamplerCreate(
	VSR_Renderer* renderer,
	size_t textureIndex,
	SDL_Surface* sur,
	VSR_SamplerFlags flags)
{
	static size_t uuid = 0;
	if(uuid == 0) {uuid++;}

	VSR_Image* img = VSR_ImageCreate(
		renderer,
		sur,
		VK_FORMAT_B8G8R8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
		| VK_IMAGE_USAGE_SAMPLED_BIT
		| VK_BUFFER_USAGE_TRANSFER_DST_BIT
	);

	if(img == NULL)
	{
		VSR_Error("Failed to create image: %s",
				   VSR_GetErr());

		goto FAIL;
	}

	VSR_ImageView* imgView = VSR_ImageViewCreate(
		renderer,
		img->image,
		img->format,
		VK_IMAGE_ASPECT_COLOR_BIT
	);

	if(imgView == NULL)
	{
		VSR_ImageDestroy(renderer, img);
		VSR_Error("Failed to create imageView: %s",
				   VSR_GetErr());
		goto FAIL;
	}

	VSR_Framebuffer* framebuffer = NULL;
	if((flags & SAMPLER_FLAG_RENDER_TARGET) != 0)
	{
		framebuffer = VSR_FramebufferCreate(
			renderer,
			imgView
		);

		if (framebuffer == NULL)
		{
			VSR_ImageDestroy(renderer, img);
			VSR_ImageViewDestroy(renderer, imgView);
			VSR_Error("Failed to create framebuffer: %s",
					  VSR_GetErr());
			goto FAIL;
		}
	}

	VSR_ImageTransition(
		renderer,
		img,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);

	SUCCESS:
	{
		///////////////////////////////////
		/// populate and return sampler ///
		///////////////////////////////////
		VSR_Sampler* sampler = SDL_malloc(sizeof(VSR_Sampler));
		sampler->uuid = uuid++;
		sampler->arrayIndex = -1;
		sampler->needsUpdate = SDL_TRUE;
		sampler->framebuffer = framebuffer;

		sampler->textureIndex = textureIndex;
		sampler->image = img;
		sampler->view = imgView;
		sampler->sampler = VSR_GetTextureSampler(renderer);

		VSR_SamplerWriteToDescriptor(renderer, textureIndex, sampler);

		return sampler;
	}
	FAIL:
	return NULL;
}

void
VSR_SamplerFree(
	VSR_Renderer* renderer,
	VSR_Sampler* sampler)
{

	// wait for the last frame to finish as sampler might be in use
	vkWaitForFences(
		renderer->logicalDevice.device,
		1,
		&renderer->imageFinished[renderer->imageIndex],
		VK_TRUE,
		-1
	);

	if(sampler->framebuffer)
	{
		VSR_FramebufferDestroy(
			renderer,
			sampler->framebuffer
		);
	}

	VSR_ImageViewDestroy(
		renderer,
		sampler->view
		);

	VSR_ImageDestroy(
		renderer,
		sampler->image
	);

}

VkSampler
VSR_GetTextureSampler(
	VSR_Renderer* renderer)
{
	static VkSampler sTextureSampler;

	if(!sTextureSampler)
	{
		VkSamplerCreateInfo textureSamplerInfo = (VkSamplerCreateInfo){0};
		textureSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		textureSamplerInfo.pNext = NULL;
		textureSamplerInfo.flags = 0L;
		textureSamplerInfo.magFilter = VK_FILTER_LINEAR;
		textureSamplerInfo.minFilter = VK_FILTER_LINEAR;
		textureSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		textureSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		textureSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		textureSamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

		textureSamplerInfo.unnormalizedCoordinates = VK_FALSE;
		textureSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		textureSamplerInfo.mipLodBias = 0.f;
		textureSamplerInfo.minLod = 0.f;
		textureSamplerInfo.maxLod = 0.f;

		textureSamplerInfo.anisotropyEnable = VK_FALSE;
		textureSamplerInfo.maxAnisotropy = 0.f;

		VkResult err = vkCreateSampler(
			renderer->logicalDevice.device,
			&textureSamplerInfo,
			VSR_GetAllocator(),
			&sTextureSampler);

		if(err != VK_SUCCESS)
		{
			VSR_Error(
				"Failed to create default sampler: %s",
				VSR_VkErrorToString(err)
			);
		}
	}

	return sTextureSampler;
}


void VSR_SamplerWriteToDescriptor(
	VSR_Renderer* renderer,
	size_t index,
	VSR_Sampler* sampler
)
{
	VkDescriptorImageInfo imageInfo;
	imageInfo.sampler = sampler->sampler;
	imageInfo.imageView = sampler->view->imageView;
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet imageWrite = (VkWriteDescriptorSet){0};
	imageWrite.pNext = NULL;

	imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	imageWrite.dstSet = renderer->descriptorPool.globalSet;
	imageWrite.dstBinding = 0;
	imageWrite.dstArrayElement = index;
	imageWrite.descriptorCount = 1;
	imageWrite.pImageInfo = &imageInfo;

	if(renderer->imageFinished[renderer->currentFrame].fence
	&& renderer->generationAcquired[renderer->currentFrame] == *renderer->imageFinished[renderer->currentFrame].generation)
	{
		vkWaitForFences(
			renderer->logicalDevice.device,
			1,
			&renderer->imageFinished[renderer->currentFrame].fence,
			VK_TRUE,
			-1
		);
	}

	vkUpdateDescriptorSets(renderer->logicalDevice.device,
	                       1, &imageWrite,
	                       0, NULL);
}
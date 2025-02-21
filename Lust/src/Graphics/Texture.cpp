#include "Texture.hpp"
#include "Application.hpp"
#ifdef LUST_USES_WINDOWS
#include "D3D12Texture.hpp"
#endif
#include "VKTexture.hpp"
#include "Image.hpp"

Lust::Texture2D* Lust::Texture2D::Instantiate(const GraphicsContext* context, const std::string& path)
{
	const std::string target = "[[native]]";
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	std::shared_ptr<Image> texture;
	size_t pos = path.find(target);
	bool isTextureNative = (pos != std::string::npos);
	if(!isTextureNative)
		texture.reset(Image::CreateImage(path));
	TextureBuffer specification =
		//std::shared_ptr<Image> img, uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet,  TextureTensor tensor, uint32_t textureIndex, size_t depth
	{ texture, TextureTensor::TENSOR_2 };
	switch (api)
	{
#ifdef LUST_USES_WINDOWS
	case Lust::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12Texture2D((const D3D12Context*)(context), specification);
	}
#endif
	case Lust::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKTexture2D((const VKContext*)(context), specification);
	}
	default:
		break;
	}
	return nullptr;
}

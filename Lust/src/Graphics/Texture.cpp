#include "Texture.hpp"
#include "Application.hpp"
#include <filesystem>
#ifdef LUST_USES_WINDOWS
#include "D3D12Texture.hpp"
#endif
#include "VKTexture.hpp"
#include "Image.hpp"

std::shared_ptr<Lust::CopyPipeline> Lust::TextureLibrary::s_CopyPipeline;
bool Lust::TextureLibrary::s_IsInitialized = false;

Lust::Texture2D::Texture2D(const TextureBuffer& specs)
{
	std::filesystem::path fpath = specs.GetFilepath();
	SetName(fpath.stem().string());
}

Lust::Texture2D* Lust::Texture2D::Instantiate(const GraphicsContext* context, const std::string& filepath)
{
	std::filesystem::path p = filepath;
	const std::string target = "[[native]]";
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	std::shared_ptr<Image> texture;
	size_t pos = filepath.find(target);
	bool isTextureNative = (pos != std::string::npos);
	if(!isTextureNative)
		texture.reset(Image::CreateImage(filepath));
	TextureBuffer specification =
		//std::shared_ptr<Image> img, TextureTensor tensor, std::string filepath
	{ texture, TextureTensor::TENSOR_2, filepath };
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

Lust::Texture2D* Lust::Texture2D::Instantiate(const GraphicsContext* context, uint32_t width, uint32_t height, std::string name)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	std::shared_ptr<Image> texture;
	uint8_t* buffer = new uint8_t[width * height * 4];
	for (size_t i = 0; i < (width * height * 4); i++)
	{
		buffer[i] = 255;
	}
	texture.reset(Image::CreateImage((std::byte*)buffer, width, height, ImageFormat::PNG));
	delete[] buffer;
	TextureBuffer specification =
		//std::shared_ptr<Image> img, TextureTensor tensor, std::string filepath
	{ texture, TextureTensor::TENSOR_2, name};
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

const std::string& Lust::Texture::GetName() const
{
	return m_Name;
}

void Lust::Texture::SetName(const std::string& name)
{
	m_Name = name;
}

void Lust::TextureLibrary::InitCopyPipeline(const GraphicsContext* context)
{
	if(!s_IsInitialized)
	{
		s_CopyPipeline.reset(CopyPipeline::Instantiate(context));
		s_IsInitialized = true;
	}
}

void Lust::TextureLibrary::DestroyCopyPipeline()
{
	if (s_IsInitialized)
	{
		s_CopyPipeline.reset();
		s_IsInitialized = false;
	}
}

std::shared_ptr<Lust::CopyPipeline>* Lust::TextureLibrary::GetCopyPipeline()
{
	return &s_CopyPipeline;
}

Lust::Texture2DLibrary::Texture2DLibrary(const GraphicsContext* context) :
	m_Context(context)
{

}

Lust::Texture2DLibrary::~Texture2DLibrary()
{
	m_Textures.clear();
	m_Context = nullptr;
}

void Lust::Texture2DLibrary::Add(const std::string& name, const std::shared_ptr<Texture2D>& texture)
{
	Console::CoreAssert(!Exists(name), "Texture already exists!");
	m_Textures[name] = texture;
}

void Lust::Texture2DLibrary::Add(const std::shared_ptr<Texture2D>& texture)
{
	auto& name = texture->GetName();
	Add(name, texture);
}

std::shared_ptr<Lust::Texture2D> Lust::Texture2DLibrary::Load(const std::string& filepath)
{
	std::shared_ptr<Texture2D> texture;
	texture.reset(Texture2D::Instantiate(m_Context, filepath));
	Add(texture);
	return texture;
}

std::shared_ptr<Lust::Texture2D> Lust::Texture2DLibrary::Load(const std::string& name, const std::string& filepath)
{
	std::shared_ptr<Texture2D> texture;
	texture.reset(Texture2D::Instantiate(m_Context, filepath));
	Add(name, texture);
	return texture;
}

std::shared_ptr<Lust::Texture2D> Lust::Texture2DLibrary::Get(const std::string& name)
{
	Console::CoreAssert(Exists(name), "Texture not found!");
	return m_Textures[name];
}

bool Lust::Texture2DLibrary::Exists(const std::string& name) const
{
	return m_Textures.find(name) != m_Textures.end();
}

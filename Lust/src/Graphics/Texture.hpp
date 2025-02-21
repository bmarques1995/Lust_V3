#pragma once

#include <cstdint>
#include <string>
#include "GraphicsContext.hpp"
#include "TextureLayout.hpp"

namespace Lust
{
	class LUST_API Texture
	{
	public:
		virtual ~Texture() = default;

		virtual const TextureBuffer& GetTextureDescription() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual bool IsLoaded() const = 0;
	};

	class LUST_API Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;
		// For Vulkan, I'm using the location mapping slot and spaceSet(std140(slot, spaceSet)) to locate the texture
		// For D3D12, I'm using the heapSlot(RootSignature descriptor range index) and textureIndex(a descriptor table can handle multiple textures)
		// In the moment that I add shaders preprocessing, I will set these parameters to the shader result
		static Texture2D* Instantiate(const GraphicsContext* context, const std::string& path);
	};
}

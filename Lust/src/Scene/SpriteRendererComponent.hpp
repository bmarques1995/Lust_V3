#pragma once

#include "LustDLLMacro.hpp"
#include <MathComponents.hpp>
#include <ColorCaster.hpp>
#include "Renderer2D.hpp"
#include "SpriteSheet.hpp"

namespace Lust
{
	struct LUST_API SpriteRendererComponent
	{
		SSBOInstanceData ColorTexInfo;
		uint32_t DrawOrder = 0;
		std::shared_ptr<Sprite2DSheet> SpriteSheet;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const vec4& color);
		SpriteRendererComponent(const vec4& color0, const vec4& color1, const vec4& color2, const vec4& color3);
		
		void SetDrawOrder(uint32_t drawOrder);
		void SetTexture(uint32_t textureIndex, uint32_t samplerIndex);
		void AddSpriteSheet(std::shared_ptr<Sprite2DSheet> spriteSheet);
		void AddSpriteSheet(std::shared_ptr<Texture2D> texture, uint32_t spriteWidth, uint32_t spriteHeight);
		void SetSprite(uint32_t xIndex, uint32_t yIndex);
		void SetUV(const vec4& formattedUV);
	};
}

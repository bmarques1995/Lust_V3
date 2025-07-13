#include "SpriteRendererComponent.hpp"

Lust::SpriteRendererComponent::SpriteRendererComponent(const vec4& color)
{
	ColorTexInfo.controllerInfo = uvec4(0, 0, 0, 0);
	ColorTexInfo.texCoordsEdges = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	ColorTexInfo.edgeColors = ColorCaster::CastFloatColor(color);
}

Lust::SpriteRendererComponent::SpriteRendererComponent(const vec4& color0, const vec4& color1, const vec4& color2, const vec4& color3)
{
	ColorTexInfo.controllerInfo = uvec4(0, 0, 0, 0);
	ColorTexInfo.texCoordsEdges = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	ColorTexInfo.edgeColors = ColorCaster::CastFloatColor(color0, color1, color2, color3);
}

void Lust::SpriteRendererComponent::SetDrawOrder(uint32_t drawOrder)
{
	DrawOrder = drawOrder;
}

void Lust::SpriteRendererComponent::SetTexture(uint32_t textureIndex, uint32_t samplerIndex)
{
	ColorTexInfo.controllerInfo = uvec4(textureIndex, samplerIndex, 0, 0);
}

void Lust::SpriteRendererComponent::AddSpriteSheet(std::shared_ptr<Sprite2DSheet> spriteSheet)
{
	SpriteSheet = spriteSheet;
}

void Lust::SpriteRendererComponent::AddSpriteSheet(std::shared_ptr<Texture2D> texture, uint32_t spriteWidth, uint32_t spriteHeight)
{
	SpriteSheet.reset(new Sprite2DSheet(texture, spriteWidth, spriteHeight));
}

void Lust::SpriteRendererComponent::SetSprite(uint32_t xIndex, uint32_t yIndex)
{
	ColorTexInfo.texCoordsEdges = SpriteSheet->GetSpriteUV(xIndex, yIndex);
}

void Lust::SpriteRendererComponent::SetUV(const vec4& formattedUV)
{
	ColorTexInfo.texCoordsEdges = formattedUV;
}

#include "TilemapComponent.hpp"

Lust::TilemapComponent::TilemapComponent(uint32_t width, uint32_t height, std::shared_ptr<Sprite2DSheet> spriteSheet, const uint32_t textureIndex, const uint32_t samplerIndex, const vec2& initialPos):
	m_Width(width),
	m_Height(height),
	m_SpriteSheet(spriteSheet),
	m_InitialPos(initialPos),
	m_TextureIndex(textureIndex),
	m_SamplerIndex(samplerIndex)
{
	m_TilemapBinds.resize(m_Width * m_Height);
}

Lust::TilemapComponent::~TilemapComponent()
{
	m_TilemapBinds.clear();
}

void Lust::TilemapComponent::AddTilemapBind(const std::string& name, uint32_t xIndex, uint32_t yIndex)
{

	auto it = m_TilemapBindMap.find(name);
	if(it != m_TilemapBindMap.end())
	{
		m_TilemapBinds[xIndex * m_Width + yIndex] = { it->second.XIndex, it->second.YIndex };
	}
}

void Lust::TilemapComponent::AddTilemapBind(uint32_t spriteXIndex, uint32_t spriteYIndex, uint32_t xIndex, uint32_t yIndex)
{
	if (m_SpriteSheet->IsValidIndex(spriteXIndex, spriteYIndex))
		m_TilemapBinds[xIndex * m_Width + yIndex] = { spriteXIndex, spriteYIndex };
}

void Lust::TilemapComponent::SetInitialPosition(const vec2& pos)
{
	m_InitialPos = pos;
}

void Lust::TilemapComponent::SetTextureIndex(const uint32_t textureIndex)
{
	m_TextureIndex = textureIndex;
}

void Lust::TilemapComponent::SetSamplerIndex(const uint32_t samplerIndex)
{
	m_SamplerIndex = samplerIndex;
}

void Lust::TilemapComponent::SetTilemapBindMap(const std::unordered_map<std::string, TilemapBind>& tilemapBindMap)
{
	m_TilemapBindMap = tilemapBindMap;
}

#include "SpriteSheet.hpp"
#include "Console.hpp"

Lust::Sprite2DSheet::Sprite2DSheet(std::shared_ptr<Texture2D> texture, uint32_t spriteWidth, uint32_t spriteHeight) :
	m_Texture(texture)
{
	Console::CoreAssert(m_Texture != nullptr, "Texture is null!");
	Console::CoreAssert((m_Texture->GetWidth() % spriteWidth == 0) && (m_Texture->GetHeight() % spriteHeight == 0), "Sprite width and height must be a multiple of the texture width and height!");
	m_SpriteWidth = spriteWidth;
	m_SpriteHeight = spriteHeight;
	m_MaxXIndex = m_Texture->GetWidth() / m_SpriteWidth -1;
	m_MaxYIndex = m_Texture->GetHeight() / m_SpriteHeight -1;
}

Lust::vec4 Lust::Sprite2DSheet::GetSpriteUV(uint32_t xIndex, uint32_t yIndex, uint32_t xQuadrants, uint32_t yQuadrants) const
{
	if ((xIndex + xQuadrants - 1) > m_MaxXIndex || (yIndex + yQuadrants - 1) > m_MaxYIndex)
		Console::CoreError("Sprite index out of bounds! xIndex: {}, yIndex: {}, xQuadrants: {}, yQuadrants: {}", xIndex, yIndex, xIndex + xQuadrants, yIndex + yQuadrants);
	if (xIndex <= m_MaxXIndex && yIndex <= m_MaxYIndex)
	{
		float x = static_cast<float>(xIndex) / static_cast<float>(m_MaxXIndex + 1);
		float y = static_cast<float>(yIndex) / static_cast<float>(m_MaxYIndex + 1);
		float w = static_cast<float>(xIndex + xQuadrants) / static_cast<float>(m_MaxXIndex + 1);
		float h = static_cast<float>(yIndex + yQuadrants) / static_cast<float>(m_MaxYIndex + 1);
		return vec4(x, y, w, h);
	}
	return vec4(0.0f, 0.0f, 1.0f, 1.0f);
}

bool Lust::Sprite2DSheet::IsValidIndex(uint32_t xIndex, uint32_t yIndex) const
{
	return xIndex < m_MaxXIndex && yIndex < m_MaxYIndex;
}

uint32_t Lust::Sprite2DSheet::GetSpriteWidth() const
{
	return m_SpriteWidth;
}

uint32_t Lust::Sprite2DSheet::GetSpriteHeight() const
{
	return m_SpriteHeight;
}

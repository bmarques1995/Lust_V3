#pragma once

#include "LustDLLMacro.hpp"
#include <Texture.hpp>

namespace Lust
{
	class LUST_API Sprite2DSheet
	{
	public:
		Sprite2DSheet(std::shared_ptr<Texture2D> texture, uint32_t spriteWidth, uint32_t spriteHeight);
		virtual ~Sprite2DSheet() = default;

		/**
		 * @brief Get the texture UV
		 * @return The texture UV
		 * @param xIndex The x index
		 * @param yIndex The y index
		 * @param xQuadrants The x quadrants, that means how many columns the texture has, 1 by default
		 * @param yQuadrants The y quadrants, that means how many rows the texture has, 1 by default
		 */
		Eigen::Vector4f GetSpriteUV(uint32_t xIndex, uint32_t yIndex, uint32_t xQuadrants = 1, uint32_t yQuadrants = 1) const;

		/**
		 * @brief Get the sprite width
		 * @return The sprite width
		 */
		uint32_t GetSpriteWidth() const;
		/**
		 * @brief Get the sprite height
		 * @return The sprite height
		 */
		uint32_t GetSpriteHeight() const;

	private:
		std::shared_ptr<Texture2D> m_Texture;
		uint32_t m_SpriteWidth;
		uint32_t m_SpriteHeight;
		uint32_t m_MaxXIndex;
		uint32_t m_MaxYIndex;
	};
}
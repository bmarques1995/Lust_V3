#pragma once

#include "LustDLLMacro.hpp"
#include <cstdint>
#include <MathComponents.hpp>
#include <vector>
#include "SpriteSheet.hpp"
#include <unordered_map>

namespace Lust
{
	struct TilemapBind
	{
		uint32_t XIndex;
		uint32_t YIndex;
	};
	struct LUST_API TilemapComponent
	{
		friend class Scene;

		TilemapComponent(uint32_t width, uint32_t height, std::shared_ptr<Sprite2DSheet> spriteSheet, const uint32_t textureIndex = 1, const uint32_t samplerIndex = 1, const vec2& initialPos = {0.0f, 0.0f});// : m_InitialPos(initialPos) {}
		~TilemapComponent();

		void AddTilemapBind(const std::string& name, uint32_t xIndex, uint32_t yIndex);
		void AddTilemapBind(uint32_t spriteXIndex, uint32_t spriteYIndex, uint32_t xIndex, uint32_t yIndex);
		void SetInitialPosition(const vec2& pos); // { m_InitialPos = pos; }

		void SetTextureIndex(const uint32_t textureIndex);// { m_SpriteSheet = spriteSheet; }
		void SetSamplerIndex(const uint32_t samplerIndex);// { m_SpriteSheet = spriteSheet; }

		void SetTilemapBindMap(const std::unordered_map<std::string, TilemapBind>& tilemapBindMap); // { m_TilemapBindMap = tilemapBindMap; }

		static void LoadTilemapFromJson(const std::string& filepath, std::shared_ptr<Texture2D>* texture, std::shared_ptr<Sprite2DSheet>* spriteSheet, std::shared_ptr<TilemapComponent>* tilemapComponent);
	private:
		vec2 m_InitialPos;
		std::shared_ptr<Sprite2DSheet> m_SpriteSheet;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_TextureIndex;
		uint32_t m_SamplerIndex;

		std::vector<TilemapBind> m_TilemapBinds;
		std::unordered_map<std::string, TilemapBind> m_TilemapBindMap;
	};
}
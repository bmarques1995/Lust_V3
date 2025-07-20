#include "TilemapComponent.hpp"
#include <json/json.h>
#include "FileHandler.hpp"
#include "Application.hpp"

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

void Lust::TilemapComponent::LoadTilemapFromJson(const std::string& filepath, std::shared_ptr<Texture2D>* texture, std::shared_ptr<Sprite2DSheet>* spriteSheet, std::shared_ptr<TilemapComponent>* tilemapComponent)
{
	auto context = Application::GetInstance()->GetContext();
	Json::Value tilemapInfo;
	Json::Reader reader;
	std::string jsonResult;
	FileHandler::ReadTextFile(filepath, &jsonResult);
	reader.parse(jsonResult, tilemapInfo);

	std::vector<std::vector<std::pair<uint32_t, uint32_t>>> tiles;

	{
		size_t i = 0;
		Json::Value& myMatrix = tilemapInfo["tiles"];
		for (auto& row : myMatrix) {
			std::vector<std::pair<uint32_t, uint32_t>> rowVec;
			//const Json::Value& myVector = row[(uint32_t)i];
			for (auto& pair : row) {
				if (pair.size() == 2) {
					uint32_t first = pair[0].asInt();
					uint32_t second = pair[1].asInt();
					rowVec.emplace_back(first, second);
				}
			}
			tiles.push_back(rowVec);
			i++;
		}
	}

	size_t rowSize = tiles[0].size();
	for (auto it = tiles.begin(); it != tiles.end(); it++)
	{
		size_t innerRowSize = it->size();
		if (rowSize != innerRowSize)
		{
			return;
		}
	}

	texture->reset(Texture2D::Instantiate(context, tilemapInfo["tilemap"]["texture"].asString()));
	spriteSheet->reset(new Sprite2DSheet(*texture, tilemapInfo["tilemap"]["tileWidth"].asUInt(), tilemapInfo["tilemap"]["tileHeight"].asUInt()));
	tilemapComponent->reset(new TilemapComponent(tiles.size(), tiles[0].size(), *spriteSheet));
	vec2 pos = { tilemapInfo["start"]["x"].asFloat(), tilemapInfo["start"]["y"].asFloat() };
	(*tilemapComponent)->SetInitialPosition(pos);

	size_t xIndex = 0;
	size_t yIndex = 0;
	for (auto it = tiles.begin(); it != tiles.end(); it++)
	{
		for (auto it2 = it->begin(); it2 != it->end(); it2++)
		{
			(*tilemapComponent)->AddTilemapBind(it2->second, it2->first, xIndex, yIndex);
			xIndex++;
		}
		xIndex = 0;
		yIndex++;
	}
}

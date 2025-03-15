#pragma once

#include "Player.hpp"

struct Pillar
{
	Eigen::Vector3f TopPosition = Eigen::Vector3f{ 0.0f, 10.0f, 0.0f };
	Eigen::Vector2f TopScale = Eigen::Vector2f{ 5.0f, 15.0f };

	Eigen::Vector3f BottomPosition = Eigen::Vector3f{ 10.0f, 10.0f, 0.0f };
	Eigen::Vector2f BottomScale = Eigen::Vector2f{ 5.0f, 15.0f };
};


class Level
{
public:
	Level();

	void Init();
	void OnUpdate(Lust::Timestep ts);
	void OnRender();

	Player& GetPlayer() { return m_Player; }

	void OnImGuiRender();
private:
	Player m_Player;
	bool m_GameOver = false;

	void CreatePillar(size_t index, float offset);
	float floorDistance = 34.0f;
	float m_PillarTarget = 30.0f;
	int m_PillarIndex = 0;

	std::vector<Pillar> m_Pillars;

	std::shared_ptr<Lust::Texture2D> m_TriangleTexture;
	std::shared_ptr<Lust::Sampler> m_TriangleSampler;
};
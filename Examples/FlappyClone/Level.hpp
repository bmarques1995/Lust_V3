#pragma once

#include "Player.hpp"

class Level
{
public:
	Level();

	void Init();
	void OnUpdate(Lust::Timestep ts);
	void OnRender();

private:
	Player m_Player;
};
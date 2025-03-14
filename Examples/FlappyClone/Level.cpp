#include "Level.hpp"

Level::Level()
{
}

void Level::Init()
{
	m_Player.LoadAssets();
}

void Level::OnUpdate(Lust::Timestep ts)
{
	m_Player.OnUpdate(ts);
}

void Level::OnRender()
{
	m_Player.OnRender();
}

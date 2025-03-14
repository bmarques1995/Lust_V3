#include "Level.hpp"
#include <Renderer2D.hpp>
#include <imgui/imgui.h>

Level::Level()
{
}

void Level::Init()
{
	m_Player.LoadAssets();

	m_Pillars.resize(5);
	for (size_t i = 0; i < 5; i++)
	{
		CreatePillar(i, 10.0f * i);
	}
}

void Level::OnUpdate(Lust::Timestep ts)
{
	m_Player.OnUpdate(ts);

	if (m_Player.GetPosition().x() > m_PillarTarget)
	{
		CreatePillar(m_PillarIndex, m_PillarTarget + 20.0f);
		m_PillarIndex = (m_PillarIndex + 1) % m_Pillars.size();
		m_PillarTarget += 10.0f;
	}
}

void Level::OnRender()
{
	auto playerPos = m_Player.GetPosition();

	Lust::Renderer2D::DrawQuad(Eigen::Vector2f{ playerPos.x(), -floorDistance }, Eigen::Vector2f{ 50.0f, 50.0f }, Eigen::Vector3f{ 1.0f, 0.0f, 0.0f }, "m_SmallMVP");
	Lust::Renderer2D::DrawQuad(Eigen::Vector2f{ playerPos.x(), floorDistance }, Eigen::Vector2f{ 50.0f, 50.0f }, Eigen::Vector3f{ 1.0f, 0.0f, 0.0f }, "m_SmallMVP");

	for(auto& pillar : m_Pillars)
	{
		Lust::Renderer2D::DrawQuad(pillar.TopPosition, pillar.TopScale, Eigen::Vector3f{ 0.0f, 1.0f, 1.0f }, "m_SmallMVP");
		Lust::Renderer2D::DrawQuad(pillar.BottomPosition, pillar.BottomScale, Eigen::Vector3f{ 0.0f, 1.0f, 1.0f }, "m_SmallMVP");
	}

	m_Player.OnRender();
}

void Level::OnImGuiRender()
{
	m_Player.OnImGuiRender();
	ImGui::InputFloat("FloorDistance", &floorDistance, .5f);
}

void Level::CreatePillar(size_t index, float offset)
{
	Pillar& pillar = m_Pillars[index];
	pillar.TopPosition.x() = offset;
	pillar.BottomPosition.x() = offset;

	float center = 0.0f;
	float gap = 2.0f;

	pillar.TopPosition.y() = 10.0f + center + gap * 0.5f;
	pillar.BottomPosition.y() = -10.0f + center + gap * 0.5f;
}
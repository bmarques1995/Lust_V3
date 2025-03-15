#include "Level.hpp"
#include <Renderer2D.hpp>
#include <imgui/imgui.h>
#include <Application.hpp>
#include <Conversions.hpp>

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

	auto context = Lust::Application::GetInstance()->GetContext();

	m_TriangleTexture.reset(Lust::Texture2D::Instantiate(context, "./Examples/FlappyClone/assets/textures/Triangle.png"));
	m_TriangleSampler.reset(Lust::Sampler::Instantiate(context, Lust::SamplerInfo(Lust::SamplerFilter::ANISOTROPIC, Lust::AnisotropicFactor::FACTOR_4, Lust::AddressMode::REPEAT, Lust::ComparisonPassMode::ALWAYS)));
	Lust::Renderer2D::UploadTexture2D(m_TriangleTexture, m_TriangleSampler, 2, 2);
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
	Eigen::Vector4<uint32_t> controllerInfo = Eigen::Vector4<uint32_t>(2, 2, 0, 0);
	auto playerPos = m_Player.GetPosition();

	for(auto& pillar : m_Pillars)
	{
		Lust::Renderer2D::DrawQuad(pillar.TopPosition, pillar.TopScale, 1.0f, Lust::Radians(180.0f), controllerInfo, "m_SmallMVP");
		Lust::Renderer2D::DrawQuad(pillar.BottomPosition, pillar.BottomScale, 1.0f, 0.0f, controllerInfo, "m_SmallMVP");
	}

	Lust::Renderer2D::DrawQuad(Eigen::Vector2f{ playerPos.x(), -floorDistance }, Eigen::Vector2f{ 50.0f, 50.0f }, Eigen::Vector3f{ 1.0f, 0.0f, 0.0f }, "m_SmallMVP");
	Lust::Renderer2D::DrawQuad(Eigen::Vector2f{ playerPos.x(), floorDistance }, Eigen::Vector2f{ 50.0f, 50.0f }, Eigen::Vector3f{ 1.0f, 0.0f, 0.0f }, "m_SmallMVP");

	m_Player.OnRender();
}

void Level::OnImGuiRender()
{
	static float pillarHeight = 15.0f;
	m_Player.OnImGuiRender();
	ImGui::InputFloat("FloorDistance", &floorDistance, .5f);
	ImGui::InputFloat("PillarHeight", &pillarHeight, .5f);

	for (auto& pillar : m_Pillars)
	{
		pillar.TopScale.y() = pillarHeight;
		pillar.BottomScale.y() = pillarHeight;
	}
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
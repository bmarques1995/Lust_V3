#include "Level.hpp"
#include <Renderer2D.hpp>
#include <imgui/imgui.h>
#include <Application.hpp>
#include <Conversions.hpp>
#include <algorithm>

static bool PointInTri(const Eigen::Vector2f& p, Eigen::Vector2f& p0, const Eigen::Vector2f& p1, const Eigen::Vector2f& p2)
{
	float s = p0.y() * p2.x() - p0.x() * p2.y() + (p2.y() - p0.y()) * p.x() + (p0.x() - p2.x()) * p.y();
	float t = p0.x() * p1.y() - p0.y() * p1.x() + (p0.y() - p1.y()) * p.x() + (p1.x() - p0.x()) * p.y();

	if ((s < 0) != (t < 0))
		return false;

	float A = -p1.y() * p2.x() + p0.y() * (p2.x() - p1.x()) + p0.x() * (p1.y() - p2.y()) + p1.x() * p2.y();

	return A < 0 ?
		(s <= 0 && s + t >= A) :
		(s >= 0 && s + t <= A);
}

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
	Lust::Renderer2D::UploadTexture2D(m_TriangleTexture, 2);
	Lust::Renderer2D::UploadSampler(m_TriangleSampler, 2);
}

void Level::OnUpdate(Lust::Timestep ts)
{
	m_Player.OnUpdate(ts);

	if (CollisionTest())
	{
		tempCollided = true;
	}

	else
	{
		tempCollided = false;
	}

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

	Lust::Renderer2D::DrawQuad(Eigen::Vector2f{ playerPos.x(), -floorDistance }, Eigen::Vector2f{ 50.0f, 50.0f }, Eigen::Vector3f{ 1.0f, 1.0f, 1.0f }, "m_SmallMVP");
	Lust::Renderer2D::DrawQuad(Eigen::Vector2f{ playerPos.x(), floorDistance }, Eigen::Vector2f{ 50.0f, 50.0f }, Eigen::Vector3f{ 1.0f, 1.0f, 1.0f }, "m_SmallMVP");

	for(auto& pillar : m_Pillars)
	{
		Lust::Renderer2D::DrawQuad(pillar.TopPosition, pillar.TopScale, 1.0f, Lust::Radians(180.0f), controllerInfo, "m_SmallMVP");
		Lust::Renderer2D::DrawQuad(pillar.BottomPosition, pillar.BottomScale, 1.0f, 0.0f, controllerInfo, "m_SmallMVP");
	}

	m_Player.OnRender();
}

void Level::OnImGuiRender()
{
	static float pillarHeight = 15.0f;
	m_Player.OnImGuiRender();
	ImGui::InputFloat("FloorDistance", &floorDistance, .5f);
	ImGui::InputFloat("PillarHeight", &pillarHeight, .5f);

	if(tempCollided)
		ImGui::Text("You Collided!");
	else
		ImGui::Text("You didn't collide!");

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
	pillar.TopPosition.z() = -.02f * index + .5f;
	pillar.BottomPosition.z() = -.02f * index + .5f;

	float center = 0.0f;
	float gap = 2.0f;

	pillar.TopPosition.y() = 10.0f + center + gap * 0.5f;
	pillar.BottomPosition.y() = -10.0f + center + gap * 0.5f;
}

bool Level::CollisionTest()
{
	if (std::abs(m_Player.GetPosition().y()) > 8.5f)
		return true;

	Eigen::Vector4f playerVertices[4] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f }
	};

	const auto& pos = m_Player.GetPosition();
	Eigen::Vector4f playerTransformedVerts[4];
	for (int i = 0; i < 4; i++)
	{
		Eigen::Quaternionf q(Eigen::AngleAxisf(Lust::Radians(m_Player.GetRotation()), Eigen::Vector3f(0.0f, 0.0f, 1.0f)));
		Eigen::Transform<float, 3, Eigen::Affine, Eigen::ColMajor> element_transform = Eigen::Translation<float, 3>({ pos.x(), pos.y(), 0.0f }) * q * Eigen::Scaling(Eigen::Vector3f{ 1.3f, 1.3f, 1.0f });

		playerTransformedVerts[i] = element_transform.matrix()
			* playerVertices[i];
	}


	// To match Triangle.png (each corner is 10% from the texture edge)
	Eigen::Vector4f pillarVertices[3] = {
		{ -0.5f + 0.1f, -0.5f + 0.1f, 0.0f, 1.0f },
		{  0.5f - 0.1f, -0.5f + 0.1f, 0.0f, 1.0f },
		{  0.0f + 0.0f,  0.5f - 0.1f, 0.0f, 1.0f },
	};

	for (auto& p : m_Pillars)
	{
		Eigen::Vector2f tri[3];

		// Top pillars
		for (int i = 0; i < 3; i++)
		{
			Eigen::Quaternionf q(Eigen::AngleAxisf(Lust::Radians(180.0f), Eigen::Vector3f(0.0f, 0.0f, 1.0f)));
			Eigen::Transform<float, 3, Eigen::Affine, Eigen::ColMajor> element_transform = Eigen::Translation<float, 3>({ p.TopPosition.x(), p.TopPosition.y(), 0.0f}) * Eigen::Scaling(Eigen::Vector3f{ p.TopScale.x(), p.TopScale.y(), 1.0f}) * q;
			tri[i] = (element_transform.matrix()	* pillarVertices[i]).block<2, 1>(0, 0);
		}

		for (auto& vert : playerTransformedVerts)
		{
			if (PointInTri({ vert.x(), vert.y()}, tri[0], tri[1], tri[2]))
				return true;
		}

		// Bottom pillars
		for (int i = 0; i < 3; i++)
		{
			Eigen::Transform<float, 3, Eigen::Affine, Eigen::ColMajor> element_transform = Eigen::Translation<float, 3>({ p.BottomPosition.x(), p.BottomPosition.y(), 0.0f }) * Eigen::Scaling(Eigen::Vector3f{ p.BottomScale.x(), p.BottomScale.y(), 1.0f });
			tri[i] = (element_transform.matrix()
				* pillarVertices[i]).block<2, 1>(0, 0);
		}

		for (auto& vert : playerTransformedVerts)
		{
			if (PointInTri({ vert.x(), vert.y()}, tri[0], tri[1], tri[2]))
				return true;
		}

	}
	return false;
}

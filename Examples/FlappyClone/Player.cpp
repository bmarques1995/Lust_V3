#include "Player.hpp"
#include <Application.hpp>
#include <Renderer2D.hpp>
#include <Input.hpp>
#include <KeyCodes.hpp>
#include <GamepadCodes.hpp>
#include <algorithm>
#include <imgui/imgui.h>
#include <Conversions.hpp>

Player::Player()
{
}

void Player::LoadAssets()
{
	auto context = Lust::Application::GetInstance()->GetContext();
	//SamplerFilter filter, AnisotropicFactor anisotropicFactor, AddressMode addressMode, ComparisonPassMode comparisonPassMode
	Lust::SamplerInfo sInfo(Lust::SamplerFilter::NEAREST, Lust::AnisotropicFactor::FACTOR_4, Lust::AddressMode::REPEAT, Lust::ComparisonPassMode::ALWAYS);
	m_ShipTexture.reset(Lust::Texture2D::Instantiate(context, "./Examples/FlappyClone/assets/textures/LustShip.png"));
	m_ShipSampler.reset(Lust::Sampler::Instantiate(context, sInfo));
	m_Velocity.x() = 10.0f;
	Lust::Renderer2D::UploadTexture2D(m_ShipTexture, m_ShipSampler, 1, 1);
}

void Player::OnRender()
{
	Eigen::Vector4<uint32_t> controllerInfo = Eigen::Vector4<uint32_t>( 1, 0, 0, 0 );
	float rotation = (m_Velocity.y() * m_AngleGain) - 90.0f;
	auto context = Lust::Application::GetInstance()->GetContext();
	Lust::Renderer2D::DrawQuad(m_Position, m_Size, 1.0f, Lust::Radians(rotation), controllerInfo, "m_SmallMVP");
} 

void Player::OnUpdate(Lust::Timestep ts)
{
	if (m_Running)
	{
		if (Lust::Input::IsKeyPressed(Lust::Key::LUST_KEYCODE_SPACE) || Lust::Input::IsGamepadKeyPressed(Lust::Gamepad::LUST_GAMEPAD_BUTTON_SOUTH))
		{
			m_Velocity.y() += m_EnginePower;
			if (m_Velocity.y() < 0.0f)
				m_Velocity.y() += m_EnginePower * 2.0f;
		}
		else
		{
			m_Velocity.y() -= m_Gravity;
		}

		m_Velocity.y() = std::clamp(m_Velocity.y(), -m_MaxVelocity, m_MaxVelocity);
 		m_Position += m_Velocity * ts;
	}
}

void Player::OnImGuiRender()
{
	ImGui::DragFloat("Engine Power", &m_EnginePower, 0.1f);
	ImGui::DragFloat("Gravity", &m_Gravity, 0.1f);
	ImGui::Checkbox("Running", &m_Running);
}

Eigen::Vector2f Player::GetPosition()
{
	return m_Position;
}

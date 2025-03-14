#include "Player.hpp"
#include <Application.hpp>
#include <Renderer2D.hpp>

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
	Lust::Renderer2D::UploadTexture2D(m_ShipTexture, m_ShipSampler);
}

void Player::OnRender()
{
	
	auto context = Lust::Application::GetInstance()->GetContext();
	Lust::Renderer2D::DrawQuad(m_Position, Eigen::Vector2f(3.0f, 3.0f), 10.0f, "m_SmallMVP");
} 

void Player::OnUpdate(Lust::Timestep ts)
{

}

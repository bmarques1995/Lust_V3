#pragma once

#include <Eigen/Eigen>
#include <Texture.hpp>
#include <Sampler.hpp>
#include <Timestep.hpp>

class Player
{
public:
	Player();

	void LoadAssets();
	void OnRender();
	void OnUpdate(Lust::Timestep ts);
private:
	Eigen::Vector2f m_Position = Eigen::Vector2f(0.0f, 0.0f);
	Eigen::Vector2f m_Velocity = Eigen::Vector2f(0.0f, 0.0f);

	std::shared_ptr<Lust::Texture2D> m_ShipTexture;
	std::shared_ptr<Lust::Sampler> m_ShipSampler;
};

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

	void OnImGuiRender();

	Eigen::Vector2f GetPosition();
private:
	Eigen::Vector2f m_Position = Eigen::Vector2f(0.0f, 0.0f);
	Eigen::Vector2f m_Velocity = Eigen::Vector2f(0.0f, 0.0f);
	Eigen::Vector2f m_Size = Eigen::Vector2f(1.3f, 1.3f);

	float m_EnginePower = 0.5f;
	float m_Gravity = 0.4f;
	float m_MaxVelocity = 20.0f;
	float m_AngleGain = 90.0f / m_MaxVelocity;
	bool m_Running = false;

	std::shared_ptr<Lust::Texture2D> m_ShipTexture;
};

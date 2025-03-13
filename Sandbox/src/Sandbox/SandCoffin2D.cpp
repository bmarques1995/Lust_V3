#include "SandCoffin2D.hpp"
#include "MathBuffer.hpp"
#include <Application.hpp>
#include <imgui/imgui.h>
#include <RenderCommand.hpp>
#include <Renderer2D.hpp>
#include <chrono>
#include <iostream>
#include <cstdio>

namespace Lust
{
	template<typename Fn>
	class Timer
	{
	public:
		Timer(const char* name, Fn function) :
			m_Name(name), m_Stopped(false), m_Function(function)
		{
			Start();
		}

		~Timer()
		{
			if (!m_Stopped)
			{
				Stop();
			}
		}

		void Start()
		{
			m_StartTimepoint = std::chrono::steady_clock::now();
		}

		void Stop()
		{
			m_Function({ m_Name, FinishClock() });
		}

		void Stop(const char* actionName)
		{
			m_Function({ actionName, FinishClock() });
		}

	private:
		float FinishClock()
		{
			auto endTimepoint = std::chrono::steady_clock::now();
			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			float duration = (end - start) * 0.001f;
			m_Stopped = true;

			return duration;
		}
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		bool m_Stopped;
		Fn m_Function;
	};
}

Lust::SandCoffin2D::SandCoffin2D() :
	Layer("SandCoffin2D")
{

}

Lust::SandCoffin2D::~SandCoffin2D()
{

}

void Lust::SandCoffin2D::OnAttach()
{
	auto window = Application::GetInstance()->GetWindow();
	auto context = Application::GetInstance()->GetContext();
	m_CameraController.reset(new OrthographicCameraController(window->GetWidth(), window->GetHeight(), true));

	SamplerInfo samplerInfoController(SamplerFilter::NEAREST, AnisotropicFactor::FACTOR_4, AddressMode::REPEAT, ComparisonPassMode::ALWAYS);
	m_Renderer2DSampler.reset(Sampler::Instantiate(context, samplerInfoController));
	m_Renderer2DTexture.reset(Texture2D::Instantiate(context, "./assets/textures/sample.png"));

	Renderer2D::UploadTexture2D(m_Renderer2DTexture, m_Renderer2DSampler);
}

void Lust::SandCoffin2D::OnDetach()
{
	m_Renderer2DTexture.reset();
	m_CameraController.reset();
}

void Lust::SandCoffin2D::OnUpdate(Timestep ts)
{
	Timer timer("SandCoffin2D::OnUpdate", [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); });
	
	timer.Start();
	m_CameraController->OnUpdate(ts);
	timer.Stop("CameraController::OnUpdate");

	timer.Start();
	Eigen::Vector2f texSize = Eigen::Vector2f((float)m_Renderer2DTexture->GetWidth() * 25, (float)m_Renderer2DTexture->GetHeight() * 25);
	timer.Stop("Get Texture Size");

	timer.Start();
	Renderer2D::BeginScene(m_CameraController->GetCamera());
	Renderer2D::DrawQuad(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector2f(150.0f, 150.0f), m_Renderer2DColor, "m_SmallMVP");
	Renderer2D::DrawQuad(Eigen::Vector3f(450.0f, 200.0f, 0.0f), Eigen::Vector2f(100.0f, 130.0f), Eigen::Vector4f(.8f, .2f, .3f, 0.0f), "m_SmallMVP");
	//Modify system, update texture and sampler once and only rebind if the texture or sampler changes
	Renderer2D::DrawQuad(Eigen::Vector3f(0.0f, 0.0f, 0.2f), texSize, 12.0f, "m_SmallMVP");
	Renderer2D::EndScene();
	timer.Stop("Renderer2D Sample Draw");
}

void Lust::SandCoffin2D::OnImGuiRender()
{
	ImGui::Begin("Color Editor");
	ImGui::ColorEdit3("Square Color", &m_Renderer2DColor(0));
	if (ImGui::Button("Reset camera"))
	{
		m_CameraController->ResetCamera();
	}
	for (auto& profile: m_ProfileResults)
	{
		char label[100];
		snprintf(label, sizeof(label), "%s %.3fms", profile.Name, profile.Time);

		ImGui::Text(label);
	}
	m_ProfileResults.clear();
	ImGui::End();
}

void Lust::SandCoffin2D::OnEvent(Event& event)
{
	m_CameraController->OnEvent(event);
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizedEvent>(std::bind(&SandCoffin2D::OnWindowResize, this, std::placeholders::_1), true);
}

bool Lust::SandCoffin2D::OnWindowResize(WindowResizedEvent& e)
{
	return false;
}

#include "SandCoffin2D.hpp"
#include "MathBuffer.hpp"
#include <Application.hpp>
#include <imgui/imgui.h>
#include <RenderCommand.hpp>
#include <Renderer2D.hpp>
#include <chrono>
#include <iostream>
#include <cstdio>
#include "Instrumentator.hpp"

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

	Renderer2D::UploadTexture2D(m_Renderer2DTexture);
	Renderer2D::UploadSampler(m_Renderer2DSampler);
}

void Lust::SandCoffin2D::OnDetach()
{
	m_Renderer2DTexture.reset();
	m_CameraController.reset();
}

void Lust::SandCoffin2D::OnUpdate(Timestep ts)
{
	Eigen::Vector2f texSize;
	{
		m_CameraController->OnUpdate(ts);
	}

	{
		texSize = Eigen::Vector2f((float)m_Renderer2DTexture->GetWidth() * 25, (float)m_Renderer2DTexture->GetHeight() * 25);
	}
	{
		Eigen::Vector4<uint32_t> controllerInfo = Eigen::Vector4<uint32_t>(1, 1, 0, 0);
		Eigen::Vector4f texCoordsEdges = Eigen::Vector4f( 0.0f, 0.0f, 1.0f, 1.0f);
		Renderer2D::BeginScene(m_CameraController->GetCamera());
		//Eigen::Vector3f m_Renderer2DColor = { .2f, 0.3f, 0.8f };
		Renderer2D::DrawQuad(Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector2f(150.0f, 150.0f), { Eigen::Vector4<uint32_t>(0, 0, 0, 0), texCoordsEdges, m_Renderer2DColor });
		Renderer2D::DrawQuad(Eigen::Vector3f(450.0f, 200.0f, 0.0f), Eigen::Vector2f(100.0f, 130.0f), { Eigen::Vector4<uint32_t>(0, 0, 0, 0), texCoordsEdges, m_Renderer2DColor2 });
		//Modify system, update texture and sampler once and only rebind if the texture or sampler changes
		Renderer2D::DrawQuad(Eigen::Vector3f(0.0f, 0.0f, 0.2f), texSize, { controllerInfo, texCoordsEdges * 12.0f, Eigen::Vector4<uint32_t>(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff) });
		Renderer2D::EndScene();
	}
}

void Lust::SandCoffin2D::OnImGuiRender()
{
	ImGui::Begin("Color Editor");
	ImGui::ColorEdit3("Square Color", &m_Renderer2DColorFloat(0));
	m_Renderer2DColor = CastFloatColor(m_Renderer2DColorFloat);
	if (ImGui::Button("Reset camera"))
	{
		m_CameraController->ResetCamera();
	}
	for (auto& profile: m_ProfileResults)
	{
		char label[100];
		snprintf(label, sizeof(label), "%.3fms %s", profile.Time, profile.Name);

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

Eigen::Vector4<uint32_t> Lust::SandCoffin2D::CastFloatColor(Eigen::Vector3f color)
{
	uint8_t r = (uint8_t)(color(0) * 255.0f);
	uint8_t g = (uint8_t)(color(1) * 255.0f);
	uint8_t b = (uint8_t)(color(2) * 255.0f);
	uint32_t pixel = (r << 24) | (g << 16) | (b << 8) | 0xff;
	return Eigen::Vector4<uint32_t>(pixel, pixel, pixel, pixel);
}

bool Lust::SandCoffin2D::OnWindowResize(WindowResizedEvent& e)
{
	return false;
}

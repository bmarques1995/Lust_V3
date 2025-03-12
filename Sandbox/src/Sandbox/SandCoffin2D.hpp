#pragma once

#include "LustSandboxDLLMacro.hpp"
#include <OrthographicCameraController.hpp>
#include <Layer.hpp>
#include <Input.hpp>
#include <Console.hpp>
#include <Texture.hpp>
#include <Sampler.hpp>
#include <Shader.hpp>
#include <ShaderReflector.hpp>
#include <Buffer.hpp>
#include "CommonDefs.hpp"

namespace Lust
{
	class LUST_SANDBOX_API SandCoffin2D : public Layer
	{
	public:
		SandCoffin2D();
		~SandCoffin2D();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;
	private:

		std::shared_ptr<OrthographicCameraController> m_CameraController;

		bool OnWindowResize(WindowResizedEvent& e);


		std::shared_ptr<Texture2D> m_Renderer2DTexture;
		std::shared_ptr<Sampler> m_Renderer2DSampler;

		Eigen::Vector3f m_Renderer2DColor = { .2f, 0.3f, 0.8f };
	};
}
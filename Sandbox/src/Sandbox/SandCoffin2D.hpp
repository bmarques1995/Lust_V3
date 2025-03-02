#pragma once

#include "LustSandboxDLLMacro.hpp"
#include <OrthographicCameraController.hpp>
#include <Layer.hpp>
#include <Input.hpp>
#include <Console.hpp>
#include <Texture.hpp>
#include <Shader.hpp>
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

		float squareVertices[3 * 4] = {
			-.5f, -.5f, .2f,
			-.5f, .5f, .2f,
			.5f, -.5f, .2f,
			.5f, .5f, .2f
		};

		uint32_t squareIndices[6] = { 3,2,1, 1,2,0 };

		std::shared_ptr<Shader> m_Renderer2DShader;
		std::shared_ptr<VertexBuffer> m_Renderer2DVertexBuffer;
		std::shared_ptr<IndexBuffer> m_Renderer2DIndexBuffer;
		std::shared_ptr<UniformBuffer> m_Renderer2DUniformBuffer;

		Eigen::Vector4f m_Renderer2DColor = { .2f, 0.3f, 0.8f, 1.0f };
	
		Sandbox::CompleteMVP m_Renderer2DCompleteMVP;
		Sandbox::SmallMVP m_Renderer2DSmallMVP;

		size_t m_Renderer2DRawSmallBufferSize = 0;
		uint8_t* m_Renderer2DRawSmallBuffer;
	};
}
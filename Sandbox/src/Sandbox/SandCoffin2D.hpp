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
#include <SpriteSheet.hpp>
#include <Sockets.hpp>
#include <Scene.hpp>
#include <Entity.hpp>

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

		Eigen::Vector4<uint32_t> CastFloatColor(Eigen::Vector3f color);

		std::shared_ptr<OrthographicCameraController> m_CameraController;

		bool OnWindowResize(WindowResizedEvent& e);

		struct ProfileResult
		{
			const char* Name;
			float Time;
		};

		std::vector<ProfileResult> m_ProfileResults;

		std::shared_ptr<Texture2D> m_Renderer2DTexture;
		std::shared_ptr<Texture2D> m_SpriteSheetTexture;
		std::shared_ptr<Sprite2DSheet> m_SpriteSheet;
		std::shared_ptr<Sampler> m_Renderer2DSampler;
		std::shared_ptr<Sockets> m_SampleSocket;
		std::shared_ptr<Scene> m_SampleScene;
		std::shared_ptr<Shader> m_Renderer2DShader;
		Lust::Entity m_SquareEntity;
		Lust::Entity m_SquareEntity2;
		Lust::Entity m_CameraEntity;

		Eigen::Vector3f m_Renderer2DColorFloat = { .2f, .3f, .8f };
		Eigen::Vector4<uint32_t> m_Renderer2DColor = { 0x334cccff, 0x334cccff, 0x334cccff, 0x334cccff };
		Eigen::Vector4<uint32_t> m_Renderer2DColor2 = { 0xcc334dff, 0xcc334dff, 0xcc334dff, 0xcc334dff };
	};
}
#include <Input.hpp>
#include <Console.hpp>
#include <Layer.hpp>
#include <Texture.hpp>
#include <Shader.hpp>
#include <Buffer.hpp>
#include <OrthographicCamera.hpp>
#include <ApplicationEvent.hpp>

namespace Lust
{
	class ExampleLayer : public Layer
	{
	public:
		ExampleLayer();

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(Timestep ts) override;

		void OnImGuiRender() override;

		void OnEvent(Event& event) override;

		void SampleInput();
	private:
		bool m_ShowDemoWindow = true;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<Texture2D> m_Texture1;
		std::shared_ptr<Texture2D> m_Texture2;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		
		std::shared_ptr<OrthographicCamera> m_Camera;
		Eigen::Vector3f m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraTranslationSpeed = 200.0f;
		float m_CameraRotation = 0.0f;
		float m_CameraRotationSpeed = 2.0f;

		bool OnWindowResize(WindowResizedEvent& e);

		Eigen::Vector<float, 9> m_VBuffer[4] =
		{
			{-.5f, -.5f, .2f, 1.0f, .0f, .0f, 1.0f,  0.0f, 1.0f },
			{-.5f, .5f, .2f, .0f, 1.0f, .0f, 1.0f,  0.0f, 0.0f },
			{.5f, -.5f, .2f, .0f, .0f, 1.0f, 1.0f,  1.0f, 1.0f},
			{.5f, .5f, .2f, 1.0f, 1.0f, .0f, 1.0f,  1.0f, 0.0f},
		};

		uint32_t iBuffer[6] =
		{
			3,2,1,
			1,2,0
		};

		struct SmallMVP
		{
			Eigen::Matrix4f model;
		};

		struct CompleteMVP
		{
			Eigen::Matrix4f model;
			Eigen::Matrix4f view;
			Eigen::Matrix4f projection;
			Eigen::Matrix4f mipLevel;
		};

		SmallMVP m_SmallMVP;
		CompleteMVP m_CompleteMVP;

	};
}
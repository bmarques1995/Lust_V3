#include <Input.hpp>
#include <Console.hpp>
#include <Layer.hpp>
#include <Texture.hpp>
#include <Shader.hpp>
#include <ShaderReflector.hpp>
#include <Buffer.hpp>
#include <OrthographicCameraController.hpp>
#include <ApplicationEvent.hpp>
#include "CommonDefs.hpp"

namespace LustLayer
{
	struct CompleteMVP
	{
		Eigen::Matrix4f model;
		Eigen::Matrix4f view;
		Eigen::Matrix4f projection;
		Eigen::Matrix4f mipLevel;
	};

	struct SmallMVP
	{
		Eigen::Matrix4f model;
	};
}

class ExampleLayer : public Lust::Layer
{
public:
	ExampleLayer();

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Lust::Timestep ts) override;

	void OnImGuiRender() override;

	void OnEvent(Lust::Event& event) override;

	void SampleInput();
private:
	bool m_ShowCameraWindow = true;

	std::shared_ptr<Lust::Shader> m_Shader;
	std::shared_ptr<Lust::ShaderReflector> m_ShaderReflector;
	std::shared_ptr<Lust::Texture2D> m_Texture1;
	std::shared_ptr<Lust::Texture2D> m_Texture2;
	std::shared_ptr<Lust::UniformBuffer> m_UniformBuffer;
	std::shared_ptr<Lust::UniformBuffer> m_UniformBuffer2;
	std::shared_ptr<Lust::VertexBuffer> m_VertexBuffer;
	std::shared_ptr<Lust::IndexBuffer> m_IndexBuffer;

	std::shared_ptr<Lust::ShaderLibrary> m_ShaderLibrary;
	std::shared_ptr<Lust::Texture2DLibrary> m_Texture2DLibrary;

	std::shared_ptr<Lust::ShaderReflector> m_SquareShaderReflector;
	std::shared_ptr<Lust::Shader> m_SquareShader;
	std::shared_ptr<Lust::VertexBuffer> m_SquareVertexBuffer;
	std::shared_ptr<Lust::IndexBuffer> m_SquareIndexBuffer;
	std::shared_ptr<Lust::UniformBuffer> m_SquareUniformBuffer;
	std::shared_ptr<Lust::StructuredBuffer> m_SquareStructuredBuffer;

	std::shared_ptr<Lust::OrthographicCameraController> m_CameraController;

	uint8_t* m_SSBO = nullptr;

	bool OnWindowResize(Lust::WindowResizedEvent& e);

	Eigen::Vector<float, 9> m_VBuffer[4] =
	{
		{-.5f, -.5f, .2f, 1.0f, .0f, .0f, 1.0f,  0.0f, 1.0f },
		{-.5f, .5f, .2f, .0f, 1.0f, .0f, 1.0f,  0.0f, 0.0f },
		{.5f, -.5f, .2f, .0f, .0f, 1.0f, 1.0f,  1.0f, 1.0f},
		{.5f, .5f, .2f, 1.0f, 1.0f, .0f, 1.0f,  1.0f, 0.0f},
	};

	uint32_t m_IBuffer[6] =
	{
		3,2,1,
		1,2,0
	};

	float squareVertices[5 * 4] = {
		-.5f, -.5f, .2f, 0.0f, 1.0f,
		-.5f, .5f, .2f, 0.0f, 0.0f,
		.5f, -.5f, .2f, 1.0f, 1.0f,
		.5f, .5f, .2f, 1.0f, 0.0f
	};

	uint32_t squareIndices[6] = { 3,2,1, 1,2,0 };

	LustLayer::SmallMVP m_SmallMVP;
	LustLayer::CompleteMVP m_CompleteMVP;
	LustLayer::SmallMVP m_SquareSmallMVP;
	//#f1c40f
	Eigen::Vector4f m_SquareColor = { 0xf1 / 255.0f, 0xc4 / 255.0f, 0x0f / 255.0f, 1.0f };
	//#9b59b6
	Eigen::Vector4f m_SquareColor2 = { 0x9b / 255.0f, 0x59 / 255.0f, 0Xb6 / 255.0f, 1.0f };
};

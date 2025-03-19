#pragma once

#include "OrthographicCamera.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include <memory>

namespace Lust
{
	/**
	* @brief Struct for scene data
	* @details Contains the model, view, projection and mip level matrices
	* @note This struct is used to store the scene data
	*/
	struct SceneData
	{
		Eigen::Matrix4f model;
		Eigen::Matrix4f view;
		Eigen::Matrix4f projection;
		Eigen::Matrix4f mipLevel;
	};

	/**
	* @brief %Renderer class
	* @details Handles rendering operations, like submitting shaders, buffers, etc.
	*/
	class LUST_API Renderer
	{
	public:
		/**
		* @brief Initializes the renderer
		*/
		static void Init();
		/**
		* @brief Shuts down the renderer
		*/
		static void Shutdown();

		/**
		* @brief Called when the window is resized
		*/
		static void OnWindowResize(uint32_t width, uint32_t height);

		/**
		* @brief Begins the scene
		*/
		static void BeginScene(const OrthographicCamera& camera);
		/**
		* @brief Ends the scene
		*/
		static void EndScene();

		/**
		* @brief Submits a SmallBufferElement
		* @param shader The shader to submit
		* @param data The data to submit
		* @param size The size of the data
		* @param element The element to submit
		* @param offset The offset of the data
		*/
		static void SubmitSmallBuffer(const std::shared_ptr<Shader>& shader, const void* data, size_t size, const SmallBufferElement& element, size_t offset = 0);
		/**
		* @brief Submits a uniform/constant buffer
		* @param uniformBuffer The uniform/constant buffer to submit
		*/
		static void SubmitCBV(std::shared_ptr<UniformBuffer>* uniformBuffer);
		/**
		* @brief Submits a shader
		* @param shader The shader to submit
		* @param vertexBuffer The vertex buffer to submit
		* @param indexBuffer The index buffer to submit
		*/
		static void SubmitShader(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer);

	private:
		

		static std::unique_ptr<SceneData> s_SceneData;
	};
}
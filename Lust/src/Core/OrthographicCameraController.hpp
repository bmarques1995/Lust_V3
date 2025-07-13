#pragma once

#include "LustDLLMacro.hpp"
#include "InputEvent.hpp"
#include "ApplicationEvent.hpp"
#include "Timestep.hpp"
#include "OrthographicCamera.hpp"
#include <MathComponents.hpp>

namespace Lust
{
	/**
	* @brief %OrthographicCameraController class, the controller of the OrthographicCamera
	* @details This class is used to control the OrthographicCamera, rotating, translating and scaling
	*/
	class LUST_API OrthographicCameraController
	{
	public:
		/**
		* @brief OrthographicCameraController constructor
		* @param width width of the window
		* @param height height of the window
		* @param rotation if the camera should rotate
		*/
		OrthographicCameraController(float width, float height, bool rotation = false);
		/**
		* @brief OnUpdate
		* @details updates the camera
		*/
		void OnUpdate(Timestep ts);
		/**
		* @brief OnEvent
		* @details resolves events
		*/
		void OnEvent(Event& e);

		/**
		* @brief GetCamera
		* @details retrieves the current OrthographicCamera
		*/
		const OrthographicCamera& GetCamera() const;
		/**
		* @brief ResetCamera
		* @details resets the camera
		*/
		void ResetCamera();

		/**
		* @brief SetPositionAndRotation
		* @details sets the camera position and rotation
		*/
		void SetPositionAndRotation(vec3 position, float rotation);
	private:

		/**
		* @brief OnMouseScroll
		* @details resolves mouse scroll events
		*/
		bool OnMouseScroll(MouseScrolledEvent& e);
		/**
		* @brief OnWindowResize
		* @details resolves window resize events
		*/
		bool OnWindowResize(WindowResizedEvent& e);

		float m_Width;
		float m_Height;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;
		bool m_Rotation;

		vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		vec3 m_CameraPreviousPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraTranslationSpeed = 200.0f;
		float m_CameraRotation = 0.0f;
		float m_CameraPreviousRotation = 0.0f;
		float m_CameraRotationSpeed = 1.0f;
	};
}
#pragma once

#include "LustDLLMacro.hpp"
#include "InputEvent.hpp"
#include "ApplicationEvent.hpp"
#include "Timestep.hpp"
#include "OrthographicCamera.hpp"

namespace Lust
{
	class LUST_API OrthographicCameraController
	{
	public:

		OrthographicCameraController(float width, float height, bool rotation = false);
		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		const OrthographicCamera& GetCamera() const;
		void ResetCamera();
	private:

		bool OnMouseScroll(MouseScrolledEvent& e);
		bool OnWindowResize(WindowResizedEvent& e);

		float m_Width;
		float m_Height;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;
		bool m_Rotation;

		Eigen::Vector3f m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraTranslationSpeed = 200.0f;
		float m_CameraRotation = 0.0f;
		float m_CameraRotationSpeed = 1.0f;
	};
}
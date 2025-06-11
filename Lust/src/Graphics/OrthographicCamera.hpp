#pragma once

#include <MathComponents.hpp>
#include "LustDLLMacro.hpp"

namespace Lust
{
	/**
	* @brief Interface for an orthographic camera
	* @details Handles camera operations, like moving, rotation, etc.
	*/
	class LUST_API OrthographicCamera
	{
	public:
		/**
		* @brief Orthographic camera constructor
		* @details Creates an orthographic camera
		* @param left The left side of the camera
		* @param right The right side of the camera
		* @param bottom The bottom of the camera
		* @param top The top of the camera
		*/
		OrthographicCamera(float left, float right, float bottom, float top);
	
		/**
		* @brief Resets the projection matrix of the camera
		* @param left The left side of the camera
		* @param right The right side of the camera
		* @param bottom The bottom of the camera
		* @param top The top of the camera
		*/
		void SetProjection(float left, float right, float bottom, float top);

		/**
		* @brief Sets the position of the camera
		* @param position The position of the camera
		*/
		void SetPosition(const vec3& position);
		/**
		* @brief Sets the rotation of the camera
		* @param rotation The rotation of the camera
		*/
		void SetRotation(float rotation);

		/**
		* @brief Gets the position of the camera
		* @return The position of the camera
		*/
		const vec3& GetPosition() const;
		/**
		* @brief Gets the rotation of the camera
		* @return The rotation of the camera
		*/
		float GetRotation(float rotation) const;

		/**
		* @brief Sets the position and rotation of the camera
		* @param position The position of the camera
		* @param rotation The rotation of the camera
		*/
		void SetPositionAndRotation(const vec3& position, float rotation);

		/**
		* @brief Gets the projection, view and view projection matrices of the camera
		*/
		const mat4& GetProjectionMatrix() const;
		/**
		* @brief Gets the view and view projection matrices of the camera
		*/
		const mat4& GetViewMatrix() const;
		/**
		* @brief Gets the view projection matrix of the camera
		*/
		const mat4& GetViewProjectionMatrix() const;

	private:
		/**
		* @brief Recalculates the view matrix
		*/
		void RecalculateViewMatrix();

		mat4 m_ProjectionMatrix;
		mat4 m_ViewMatrix;
		mat4 m_ViewProjectionMatrix;

		vec3 m_CameraPosition;
		float m_Rotation;
	};
}
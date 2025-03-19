#pragma once

#include <Eigen/Eigen>
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
		void SetPosition(const Eigen::Vector3f& position);
		/**
		* @brief Sets the rotation of the camera
		* @param rotation The rotation of the camera
		*/
		void SetRotation(float rotation);

		/**
		* @brief Gets the position of the camera
		* @return The position of the camera
		*/
		const Eigen::Vector3f& GetPosition() const;
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
		void SetPositionAndRotation(const Eigen::Vector3f& position, float rotation);

		/**
		* @brief Gets the projection, view and view projection matrices of the camera
		*/
		const Eigen::Matrix4f& GetProjectionMatrix() const;
		/**
		* @brief Gets the view and view projection matrices of the camera
		*/
		const Eigen::Matrix4f& GetViewMatrix() const;
		/**
		* @brief Gets the view projection matrix of the camera
		*/
		const Eigen::Matrix4f& GetViewProjectionMatrix() const;

	private:
		/**
		* @brief Recalculates the view matrix
		*/
		void RecalculateViewMatrix();

		Eigen::Matrix4f m_ProjectionMatrix;
		Eigen::Matrix4f m_ViewMatrix;
		Eigen::Matrix4f m_ViewProjectionMatrix;

		Eigen::Vector3f m_CameraPosition;
		float m_Rotation;
	};
}
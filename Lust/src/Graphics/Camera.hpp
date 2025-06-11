#pragma once

#include "LustDLLMacro.hpp"
#include <MathComponents.hpp>

namespace Lust
{
	class LUST_API Camera
	{
	public:
		Camera(const mat4& projectionMatrix, const mat4& viewMatrix);

		const mat4& GetProjectionMatrix() const;
		const mat4& GetViewMatrix() const;

		void ResetProjectionMatrix(const mat4& projectionMatrix);
		void ResetViewMatrix(const mat4& viewMatrix);
	private:
		mat4 m_ProjectionMatrix = mat4::Identity();
		mat4 m_ViewMatrix = mat4::Identity();
	};
}
#pragma once

#include "CopyPipeline.hpp"

namespace Lust
{
	class Drawer
	{
	public:
		virtual void Draw(uint32_t count) = 0;
		virtual void DrawIndexed(uint32_t count) = 0;

		static Drawer* Instantiate(const std::shared_ptr<GraphicsContext>* context);
	};
}
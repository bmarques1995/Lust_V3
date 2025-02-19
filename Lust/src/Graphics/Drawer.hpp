#pragma once

#include "CopyPipeline.hpp"

namespace Lust
{
	class Drawer
	{
	public:
		virtual void Draw(uint32_t count, uint32_t instances = 1) = 0;
		virtual void DrawIndexed(uint32_t count, uint32_t instances = 1) = 0;

		static Drawer* Instantiate(const GraphicsContext* context);
	};
}
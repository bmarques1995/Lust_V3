#pragma once

#include "Drawer.hpp"
#include "GraphicsContext.hpp"

namespace Lust {

	class LUST_API RenderCommand
	{
	public:
		static void Init(const GraphicsContext* context)
		{
			s_Drawer.reset(Drawer::Instantiate(context));
		}

		static void Shutdown()
		{
			s_Drawer.reset();
		}

		static void Draw(uint32_t elementCount, uint32_t instanceCount = 1)
		{
			s_Drawer->Draw(elementCount, instanceCount);
		}

		static void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1)
		{
			s_Drawer->DrawIndexed(indexCount, instanceCount);
		}
		
	private:
		static std::unique_ptr<Drawer> s_Drawer;
	};

}
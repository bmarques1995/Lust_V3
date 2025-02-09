#pragma once

#include "Drawer.hpp"
#include "GraphicsContext.hpp"

namespace Lust {

	class LUST_API RenderCommand
	{
	public:
		static void Init(const std::shared_ptr<GraphicsContext>* context)
		{
			s_Drawer.reset(Drawer::Instantiate(context));
		}

		static void Shutdown()
		{
			s_Drawer.reset();
		}

		static void Draw(uint32_t elementCount)
		{
			s_Drawer->Draw(elementCount);
		}

		static void DrawIndexed(uint32_t indexCount)
		{
			s_Drawer->DrawIndexed(indexCount);
		}
		
	private:
		static std::unique_ptr<Drawer> s_Drawer;
	};

}
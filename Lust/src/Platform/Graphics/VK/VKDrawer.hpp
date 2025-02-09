#pragma once

#ifdef LUST_USES_WINDOWS

#include "Drawer.hpp"
#include "VKContext.hpp"

namespace Lust
{
	class LUST_API VKDrawer : public Drawer
	{
	public:
		VKDrawer(const std::shared_ptr<VKContext>* context);
		~VKDrawer();

		void Draw(uint32_t count) override;
		void DrawIndexed(uint32_t count) override;
	private:
		const std::shared_ptr<VKContext>* m_Context;
	};
}

#endif
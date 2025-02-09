#pragma once

#ifdef LUST_USES_WINDOWS

#include "Drawer.hpp"
#include "VKContext.hpp"

namespace Lust
{
	class LUST_API VKDrawer : public Drawer
	{
	public:
		VKDrawer(const VKContext* context);
		~VKDrawer();

		void Draw(uint32_t count) override;
		void DrawIndexed(uint32_t count) override;
	private:
		const VKContext* m_Context;
	};
}

#endif
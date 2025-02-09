#pragma once

#ifdef LUST_USES_WINDOWS

#include "Drawer.hpp"
#include "D3D12Context.hpp"

namespace Lust
{
	class LUST_API D3D12Drawer : public Drawer
	{
	public:
		D3D12Drawer(const std::shared_ptr<D3D12Context>* context);
		~D3D12Drawer();

		void Draw(uint32_t count) override;
		void DrawIndexed(uint32_t count) override;
	private:
		const std::shared_ptr<D3D12Context>* m_Context;
	};
}

#endif

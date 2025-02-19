#include "D3D12Drawer.hpp"

Lust::D3D12Drawer::D3D12Drawer(const D3D12Context* context) :
	m_Context(context)
{
}

Lust::D3D12Drawer::~D3D12Drawer()
{
}

void Lust::D3D12Drawer::Draw(uint32_t count, uint32_t instances)
{
	auto cmdList = m_Context->GetCurrentCommandList();
	cmdList->DrawInstanced(count, instances, 0, 0);
}

void Lust::D3D12Drawer::DrawIndexed(uint32_t count, uint32_t instances)
{
	auto cmdList = m_Context->GetCurrentCommandList();
	cmdList->DrawIndexedInstanced(count, instances, 0, 0, 0);
}

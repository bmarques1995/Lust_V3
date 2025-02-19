#include "VKDrawer.hpp"

Lust::VKDrawer::VKDrawer(const VKContext* context) :
	m_Context(context)
{
}

Lust::VKDrawer::~VKDrawer()
{
}

void Lust::VKDrawer::Draw(uint32_t count, uint32_t instances)
{
	auto cmdBuffer = m_Context->GetCurrentCommandBuffer();
	vkCmdDraw(cmdBuffer, count, instances, 0, 0);
}

void Lust::VKDrawer::DrawIndexed(uint32_t count, uint32_t instances)
{
	auto cmdBuffer = m_Context->GetCurrentCommandBuffer();
	vkCmdDrawIndexed(cmdBuffer, count, instances, 0, 0, 0);
}

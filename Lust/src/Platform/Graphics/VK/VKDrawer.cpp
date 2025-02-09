#include "VKDrawer.hpp"

Lust::VKDrawer::VKDrawer(const std::shared_ptr<VKContext>* context) :
	m_Context(context)
{
}

Lust::VKDrawer::~VKDrawer()
{
}

void Lust::VKDrawer::Draw(uint32_t count)
{
	auto cmdBuffer = (*m_Context)->GetCurrentCommandBuffer();
	vkCmdDraw(cmdBuffer, count, 1, 0, 0);
}

void Lust::VKDrawer::DrawIndexed(uint32_t count)
{
	auto cmdBuffer = (*m_Context)->GetCurrentCommandBuffer();
	vkCmdDrawIndexed(cmdBuffer, count, 1, 0, 0, 0);
}

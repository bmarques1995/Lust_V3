#include "VKDrawer.hpp"
#include "VKFunctions.hpp"

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
	VKFunctions::vkCmdDrawFn(cmdBuffer, count, instances, 0, 0);
}

void Lust::VKDrawer::DrawIndexed(uint32_t count, uint32_t instances)
{
	auto cmdBuffer = m_Context->GetCurrentCommandBuffer();
	VKFunctions::vkCmdDrawIndexedFn(cmdBuffer, count, instances, 0, 0, 0);
}

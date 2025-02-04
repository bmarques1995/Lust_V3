#pragma	once

#include "LustDLLMacro.hpp"
#include "ImguiContext.hpp"
#include "VKContext.hpp"
#include <imgui_impl_vulkan.h>

namespace Lust
{
	class VKImguiContext : public ImguiContext
	{
	public:
		VKImguiContext(const std::shared_ptr<VKContext>* vkContext);
		~VKImguiContext();

		void ReceiveInput() override;
		void DispatchInput() override;
	private:
		const std::shared_ptr<VKContext>* m_Context;
#ifdef LUST_USES_WINDOWS
		static int ImGui_ImplWin32_CreateVkSurface(ImGuiViewport* viewport, ImU64 vk_instance, const void* vk_allocator, ImU64* out_vk_surface);
#endif
	};
}
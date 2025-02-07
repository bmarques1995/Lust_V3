#pragma once

#include "CopyPipeline.hpp"
#include "VKContext.hpp"

namespace Lust
{
	class LUST_API VKCopyPipeline : public CopyPipeline
	{
	public:
		VKCopyPipeline(const std::shared_ptr<VKContext>* context);
		~VKCopyPipeline();

		void Wait() override;
	
		VkQueue GetCopyQueue() const;
		VkCommandBuffer GetCommandBuffer() const;
		VkCommandPool GetCommandPool() const;

	private:
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice adapter);
		const std::shared_ptr<VKContext>* m_Context;

		VkCommandPool m_CopyCommandPool;
		VkCommandBuffer m_CopyCommandBuffer;
		VkQueue m_CopyQueue;
	};
}
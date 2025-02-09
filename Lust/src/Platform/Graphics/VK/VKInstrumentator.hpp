#pragma once

#include "VKContext.hpp"
#include "GPUInstrumentator.hpp"
#include <memory>

namespace Lust
{
	class LUST_API VKInstrumentator : public GPUInstrumentator
	{
	public:
		VKInstrumentator(const VKContext* context);
		~VKInstrumentator();

		void BeginQueryTime() override;
		void EndQueryTime() override;

		double GetQueryTime() override;
	private:
		const VKContext* m_Context;
		VkQueryPool m_QueryPool;
		uint32_t m_QueryCount;
		uint64_t* m_Timestamp;
		double m_TimestampPeriod;
	};
}

#pragma once

#include "LustDLLMacro.hpp"
#include "GraphicsContext.hpp"
#include <memory>

namespace Lust
{
	class LUST_API GPUInstrumentator
	{
	public:
		virtual ~GPUInstrumentator() = default;

		virtual void BeginQueryTime() = 0;
		virtual void EndQueryTime() = 0;

		virtual double GetQueryTime() = 0;

		static GPUInstrumentator* Instantiate(const std::shared_ptr<GraphicsContext>* context);
	};
}
#pragma once
#include "LustDLLMacro.hpp"
#include "GraphicsContext.hpp"
#include <memory>

namespace Lust
{
	class LUST_API CopyPipeline
	{
	public:
		virtual ~CopyPipeline() = default;

		virtual void Wait() = 0;

		static CopyPipeline* Instantiate(const std::shared_ptr<GraphicsContext>* context);
	};
}
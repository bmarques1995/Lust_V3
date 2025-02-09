#pragma once

#ifdef LUST_USES_WINDOWS

#include "CopyPipeline.hpp"
#include "D3D12Context.hpp"

namespace Lust
{
	class D3D12CopyPipeline : public CopyPipeline
	{
	public:
		D3D12CopyPipeline(const D3D12Context* context);
		~D3D12CopyPipeline();

		void Wait() override;
	
		ID3D12GraphicsCommandList10* GetCommandList() const;
		ID3D12CommandAllocator* GetCommandAllocator() const;
		ID3D12CommandQueue* GetCommandQueue() const;
	
	private:
		ComPointer<ID3D12CommandAllocator> m_CopyCommandAllocator;
		ComPointer<ID3D12CommandQueue> m_CopyCommandQueue;
		ComPointer<ID3D12GraphicsCommandList10> m_CopyCommandList;
		ComPointer<ID3D12Fence> m_CopyFence;
		uint64_t m_CopyFenceValue = 0;
		HANDLE m_CopyFenceEvent;

		void WaitCopyPipeline(UINT64 fenceValue = -1);

		const D3D12Context* m_Context;
	};
}

#endif

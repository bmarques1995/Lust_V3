#pragma once

#ifdef LUST_USES_WINDOWS

#include "GraphicsContext.hpp"
#include "ComPointer.hpp"

#include <windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>

#ifdef LUST_DEBUG_MODE
#include <dxgidebug.h>
#include <d3d12sdklayers.h>
#endif

namespace Lust
{
	class LUST_API D3D12Context : public GraphicsContext
	{
	public:
		D3D12Context(const Window* windowHandle, uint32_t framesInFlight);
		~D3D12Context();

		void SetClearColor(float r, float g, float b, float a) override;

		uint32_t GetUniformAttachment() const override;

		void ReceiveCommands() override;
		void DispatchCommands() override;
		void Present() override;
		void StageViewportAndScissors() override;

		void SetVSync(bool enableVSync) override;
		bool IsVSyncEnabled() const override;

		uint32_t GetSmallBufferAttachment() const override;
		uint32_t GetFramesInFlight() const override;

		void Draw(uint32_t elements) override;

		ID3D12Device14* GetDevicePtr() const;
		ID3D12GraphicsCommandList10* GetCurrentCommandList() const;
		ID3D12CommandQueue* GetCommandQueue() const;
		D3D_FEATURE_LEVEL GetFeatureLevel() const;

		const std::string GetGPUName() override;

		void WindowResize(uint32_t width, uint32_t height) override;

	private:
		void CreateFactory();
		void CreateAdapter();
		void CreateDevice();
		void CreateCommandQueue();
		void CreateSwapChain(HWND windowHandle);
		void CreateRenderTargetView();
		void CreateCommandAllocator();
		void CreateCommandLists();
		void CreateViewportAndScissor(uint32_t width, uint32_t height);
		void CreateDepthStencilView();

		void GetTargets();
		void FlushQueue(size_t flushCount = 1);
		void WaitForFence(UINT64 fenceValue = -1);

#ifdef LUST_DEBUG_MODE
		void EnableDebug();
		void DisableDebug();

		ComPointer<ID3D12Debug6> m_D3D12Debug;
		ComPointer<IDXGIDebug1> m_DXGIDebug;
#endif
		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		std::string m_GPUName;
		uint64_t m_RTVHeapIncrement = 0;

		ComPointer<IDXGIFactory7> m_DXGIFactory;
		ComPointer<IDXGIAdapter4> m_DXGIAdapter;

		ComPointer<ID3D12Device14> m_Device;
		ComPointer<ID3D12CommandQueue> m_CommandQueue;
		ComPointer<ID3D12Fence> m_CommandQueueFence;
		uint64_t m_CommandQueueFenceValue = 0;
		HANDLE m_CommandQueueFenceEvent = nullptr;

		bool m_IsVSyncEnabled;
		static const std::unordered_map<uint32_t, D3D_FEATURE_LEVEL> s_AvaliableFeatureLevels;

		D3D_FEATURE_LEVEL m_FeatureLevel;
		ComPointer<IDXGISwapChain4> m_SwapChain;
		ComPointer<ID3D12Resource2>* m_RenderTargets;
		ComPointer<ID3D12DescriptorHeap> m_RTVHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE* m_RTVHandles;
		ComPointer<ID3D12Resource2> m_DepthStencilView;
		ComPointer<ID3D12DescriptorHeap> m_DSVHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_DSVHandle;
		uint32_t m_FramesInFlight;
		D3D12_CLEAR_VALUE m_ClearColor;

		ComPointer<ID3D12CommandAllocator>* m_CommandAllocators;
		ComPointer<ID3D12GraphicsCommandList10>* m_CommandLists;

		UINT m_CurrentBufferIndex = -1;
	};
}

#endif
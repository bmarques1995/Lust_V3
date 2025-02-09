#ifdef LUST_USES_WINDOWS

#include "D3D12ImguiContext.hpp"
#include <functional>

Lust::DescriptorHeapAllocator* Lust::D3D12ImguiContext::s_DescriptorHeapImguiAllocator = nullptr;

Lust::D3D12ImguiContext::D3D12ImguiContext(const D3D12Context* context) :
	m_Context(context)
{
    HRESULT hr;
    auto device = m_Context->GetDevicePtr();
    const uint32_t APP_SRV_HEAP_SIZE = 64;
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = APP_SRV_HEAP_SIZE;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_ImGuiHeap.GetAddressOf()));
    assert(hr == S_OK);

    m_ImGuiHeap->SetName(L"ImGui Heap");

    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = device;
    init_info.CommandQueue = m_Context->GetCommandQueue();
    init_info.NumFramesInFlight = m_Context->GetFramesInFlight();
    init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    init_info.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    // Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
    // (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
    init_info.SrvDescriptorHeap = m_ImGuiHeap.Get();
    if (s_DescriptorHeapImguiAllocator == nullptr)
    {
        s_DescriptorHeapImguiAllocator = new DescriptorHeapAllocator();
        s_DescriptorHeapImguiAllocator->Create(init_info.Device, m_ImGuiHeap.Get());
    }
    


    init_info.SrvDescriptorAllocFn = D3D12ImguiContext::AllocateDescriptor;
    init_info.SrvDescriptorFreeFn = D3D12ImguiContext::FreeDescriptor;
    ImGui_ImplDX12_Init(&init_info);
}

Lust::D3D12ImguiContext::~D3D12ImguiContext()
{
    ImGui_ImplDX12_Shutdown();
    if (s_DescriptorHeapImguiAllocator != nullptr)
    {
        s_DescriptorHeapImguiAllocator->Destroy();
        delete s_DescriptorHeapImguiAllocator;
        s_DescriptorHeapImguiAllocator = nullptr;
    }
}

void Lust::D3D12ImguiContext::ReceiveInput()
{
    auto cmdList = m_Context->GetCurrentCommandList();
    cmdList->SetDescriptorHeaps(1, m_ImGuiHeap.GetAddressOf());
    ImGui_ImplDX12_NewFrame();
}

void Lust::D3D12ImguiContext::DispatchInput()
{
    auto cmdList = m_Context->GetCurrentCommandList();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
}

void Lust::D3D12ImguiContext::AllocateDescriptor(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
{
    s_DescriptorHeapImguiAllocator->Alloc(out_cpu_desc_handle, out_gpu_desc_handle);
}

void Lust::D3D12ImguiContext::FreeDescriptor(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
{
    s_DescriptorHeapImguiAllocator->Free(out_cpu_desc_handle, out_gpu_desc_handle);
}

void Lust::DescriptorHeapAllocator::Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
{
    IM_ASSERT(Heap == nullptr && FreeIndices.empty());
    Heap = heap;
    D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
    HeapType = desc.Type;
    HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
    HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
    HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
    FreeIndices.reserve((int)desc.NumDescriptors);
    for (int n = desc.NumDescriptors; n > 0; n--)
        FreeIndices.push_back(n);
}

void Lust::DescriptorHeapAllocator::Destroy()
{
    Heap = nullptr;
    FreeIndices.clear();
}

void Lust::DescriptorHeapAllocator::Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
{
    IM_ASSERT(FreeIndices.Size > 0);
    int idx = FreeIndices.back();
    FreeIndices.pop_back();
    out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
    out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
}

void Lust::DescriptorHeapAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
{
    int cpu_idx = (int)((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
    int gpu_idx = (int)((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
    IM_ASSERT(cpu_idx == gpu_idx);
    FreeIndices.push_back(cpu_idx);
}

#endif

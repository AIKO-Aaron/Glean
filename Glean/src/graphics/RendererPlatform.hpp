#pragma once

#ifdef _WIN32

#  include <Windows.h>
#  include <d3d12.h>
#  include <dxgi1_6.h>
#  include <wrl.h>
typedef HWND __internalWindow;
typedef struct {
    Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[NUM_FRAMEBUFFERS];
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    HANDLE fenceEvent;
    
    D3D12_VIEWPORT viewport;
    D3D12_RECT scissorRect;
    
    UINT size;
    UINT frameIndex;
    UINT64 fenceValue;
} __rendererData;

#elif defined(__APPLE__)

typedef void* __internalWindow;
typedef struct {
    __internalWindow window;
    
    void *commandQueue;
    void *commandBuffer;
    void *commandEncoder;
} __rendererData;

#else

#endif

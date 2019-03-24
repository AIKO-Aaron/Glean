#pragma once

#include <vector>

#define NUM_FRAMEBUFFERS 2

// Platform specific stuff
#if defined(_WIN32)
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

#else

#endif

namespace graphics {

	class Renderer {
	private:
		__rendererData rd;

	public:
		Renderer(__internalWindow window);

		void swapBuffers();
		void synchronize(); // Wait for frame to render (maybe not needed for every renderer)
		void clearColor(float r, float g, float b, float a);
	};

}
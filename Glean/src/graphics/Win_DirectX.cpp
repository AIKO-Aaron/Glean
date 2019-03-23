#ifndef _WIN32
#  error This Source file should only be compiled on windows!
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d3d12.h> // DirectX12 3D
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "Renderer.h"

#define NUM_FRAMEBUFFERS 3

using namespace graphics;
using namespace Microsoft::WRL;

Renderer::Renderer(HWND window) {
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debugController;
	HRESULT res = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	if (res >= 0) debugController->EnableDebugLayer();
#endif

	// Initialize the pipeline

	ComPtr<IDXGIFactory4> factory;
	CreateDXGIFactory1(IID_PPV_ARGS(&factory));

#ifdef _DEBUG_PRINT_VIDEOCARDS
	std::vector<IDXGIAdapter1*> adapters;
	IDXGIAdapter1 *hardwareAdapter;
	int i = 0;
	while (factory->EnumAdapters1(i, &hardwareAdapter) != DXGI_ERROR_NOT_FOUND) {
		adapters.push_back(hardwareAdapter);
		DXGI_ADAPTER_DESC desc;
		hardwareAdapter->GetDesc(&desc);
		printf("Another adapter found: %S\n", desc.Description);
		++i;
	}
#endif

	ComPtr<ID3D12Device> device;
	res = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)); // Use default video card
	if (res < 0) printf("[ERROR] Error creating a device for DirectX\n");

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ComPtr<ID3D12CommandQueue> commandQueue;
	res = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
	if (res < 0) printf("[ERROR] Error creating a commandqueuedesc for DirectX\n");

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = NUM_FRAMEBUFFERS; // Tripple buffer
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

	ComPtr<IDXGISwapChain1> tmpSwapChain;
	res = factory->CreateSwapChainForHwnd(commandQueue.Get(), window, &swapChainDesc, nullptr, nullptr, &tmpSwapChain);
	if (res < 0) printf("[ERROR] Error creating a swap chain for DirectX %.08X\n", res);

	ComPtr<IDXGISwapChain3> swapChain;
	if(!tmpSwapChain.As(&swapChain)) printf("[ERROR] Error creating a IDXGISwapChain3 for DirectX (Was NULL after casting)\n");
	UINT currentBuffer = swapChain->GetCurrentBackBufferIndex();


	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = NUM_FRAMEBUFFERS;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	UINT size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

	ComPtr<ID3D12Resource> renderTargets[NUM_FRAMEBUFFERS];
	for (int i = 0; i < NUM_FRAMEBUFFERS; i++) {
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, size);
	}

	ComPtr<ID3D12CommandAllocator> commandAllocator;
	res = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	if (res < 0) printf("[ERROR] Error creating a command allocator for DirectX\n");
}
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
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));

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

	res = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&rd.commandQueue));
	if (res < 0) printf("[ERROR] Error creating a commandqueuedesc for DirectX\n");

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = NUM_FRAMEBUFFERS;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
	fullscreenDesc.Windowed = TRUE;

	ComPtr<IDXGISwapChain1> tmpSwapChain;
	res = factory->CreateSwapChainForHwnd(rd.commandQueue.Get(), window, &swapChainDesc, &fullscreenDesc, nullptr, &tmpSwapChain);
	if (res < 0) printf("[ERROR] Error creating a swap chain for DirectX %.08X\n", res);

	if(tmpSwapChain.As(&rd.swapChain) < 0) printf("[ERROR] Error creating a IDXGISwapChain3 for DirectX (Was NULL after casting)\n");
	rd.frameIndex = rd.swapChain->GetCurrentBackBufferIndex();


	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = NUM_FRAMEBUFFERS;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rd.rtvHeap));
	rd.size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rd.rtvHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < NUM_FRAMEBUFFERS; i++) {
		rd.swapChain->GetBuffer(i, IID_PPV_ARGS(&rd.renderTargets[i]));
		device->CreateRenderTargetView(rd.renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rd.size);
	}

	res = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&rd.commandAllocator));
	if (res < 0) printf("[ERROR] Error creating a command allocator for DirectX\n");

	/**
	*	Now for the assets:
	*/

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rd.rootSignature));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rd.rootSignature.Get();
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&rd.pipelineState));

	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, rd.commandAllocator.Get(), rd.pipelineState.Get(), IID_PPV_ARGS(&rd.commandList));
	rd.commandList->Close();

	// TODO: Create vertex buffers

	// Synchronization stuff
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&rd.fence));
	rd.fenceValue = 1;

	rd.fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (rd.fenceEvent == nullptr) printf("[ERROR] Couldn't create an empty event for DirectX\n");

	synchronize();
}

void Renderer::synchronize() {
	const UINT64 fenceVal = rd.fenceValue;
	rd.commandQueue->Signal(rd.fence.Get(), fenceVal);
	++rd.fenceValue;
	if (rd.fence->GetCompletedValue() < fenceVal) {
		rd.fence->SetEventOnCompletion(fenceVal, rd.fenceEvent);
		WaitForSingleObject(rd.fenceEvent, INFINITE);
	}

	rd.frameIndex = rd.swapChain->GetCurrentBackBufferIndex();
}

void Renderer::swapBuffers() {
	rd.commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rd.renderTargets[rd.frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	rd.commandList->Close();

	// Execute commands in queue
	ID3D12CommandList* ppCommandLists[] = { rd.commandList.Get() };
	rd.commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Swap buffers
	rd.swapChain->Present(1, 0);
	synchronize();

	rd.commandAllocator->Reset();
	rd.commandList->Reset(rd.commandAllocator.Get(), rd.pipelineState.Get());

	rd.commandList->SetGraphicsRootSignature(rd.rootSignature.Get());
	rd.commandList->RSSetViewports(1, &rd.viewport);
	rd.commandList->RSSetScissorRects(1, &rd.scissorRect);

}

void Renderer::clearColor(float r, float g, float b, float a) {
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rd.rtvHeap->GetCPUDescriptorHandleForHeapStart(), rd.frameIndex, rd.size);
	rd.commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	rd.commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rd.renderTargets[rd.frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	const float clearColor[] = { r, g, b, a };
	rd.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}
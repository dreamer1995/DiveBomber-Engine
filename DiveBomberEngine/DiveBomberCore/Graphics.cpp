#include "Graphics.h"

Graphics::Graphics(HWND inputHWnd)
{
	hWnd = inputHWnd;
	gpuAdapter = std::make_unique<GPUAdapter>();
	dxDevice = std::make_unique<DXDevice>(gpuAdapter->GetAdapter());
	commandQueue = std::make_unique<CommandQueue>(dxDevice->GetDecive());
	swapChain = std::make_unique<SwapChain>(hWnd, commandQueue->GetCommandQueue());
}

Graphics::~Graphics()
{

}
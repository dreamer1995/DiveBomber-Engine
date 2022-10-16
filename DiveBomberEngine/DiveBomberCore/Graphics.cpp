#include "Graphics.h"

Graphics::Graphics(HWND inputHWnd)
{
	infoManager = std::make_shared<DxgiInfoManager>();
	gpuAdapter = std::make_unique<GPUAdapter>(infoManager);
	dxDevice = std::make_unique<DXDevice>(gpuAdapter->GetAdapter(), infoManager);
	commandQueue = std::make_unique<CommandQueue>(dxDevice->GetDecive(), infoManager);
}

Graphics::~Graphics()
{

}
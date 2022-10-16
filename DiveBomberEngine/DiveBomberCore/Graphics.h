#pragma once
#include "Common.h"
#include "WindowResource.h"
#include "GraphicsResource.h"
#include "GPUAdapter.h"
#include "DXDevice.h"
#include "CommandQueue.h"
#include "SwapChain.h"

class Graphics
{
public:
	Graphics(HWND inputHWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void BeginFrame(float red, float green, float blue) noexcept;
	void DrawIndexed(UINT count) noxnd;
	void DrawInstanced(UINT vertexCount, UINT instanceCount) noxnd;
	void Dispatch(UINT x, UINT y, UINT group) noxnd;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;
	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
	//std::shared_ptr<Bind::RenderTarget> GetTarget();
	void ClearShaderResources(UINT slot) noexcept;
	void UnbindTessellationShaders() noexcept;
	void ClearConstantBuffers(UINT slot) noexcept;
	void SetFOV(float FOV) noexcept;
	float GetFOV() const noexcept;
	void ClearRenderTarget() noexcept;
	void ClearUAV(UINT slot) noexcept;
	void ClearShader() noexcept;
private:
	UINT width = DefaultWindowWidth;
	UINT height = DefaultWindowHeight;
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	bool imguiEnabled = true;
	float mFOV;
#ifndef NDEBUG
	std::shared_ptr<DxgiInfoManager> infoManager;
#endif
	wrl::ComPtr<ID3D12Device2> pDevice;
	wrl::ComPtr<IDXGISwapChain4> pSwap;
	HWND hWnd;
	//wrl::ComPtr<ID3D12DeviceContext> pContext;

	//std::shared_ptr<Bind::RenderTarget> pTarget;
	std::unique_ptr<GPUAdapter> gpuAdapter;
	std::unique_ptr<DXDevice> dxDevice;
	std::unique_ptr<CommandQueue> commandQueue;
	std::unique_ptr<SwapChain> swapChain;
public:
	bool isWireFrame = false;
	bool isTAA;
	bool isHBAO;
	bool isHDR;
	//bool isVolumetricRendering;
	bool isSkyRendering;
};


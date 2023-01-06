#pragma once
#include "Common.h"
#include "WindowResource.h"
#include "GraphicsResource.h"
#include "GPUAdapter.h"
#include "DXDevice.h"
#include "CommandManager.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"
#include "FenceManager.h"

class Graphics final
{
public:
	Graphics(HWND inputHWnd, UINT includeWidth, UINT includeHeight);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
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

	void BeginFrame();
	void EndFrame();
	HANDLE GetFenceEvent() noexcept;
	void ReSizeMainRT(uint32_t inputWidth, uint32_t inputHeight);
	CommandManager* GetCommandManager() noexcept;
	void Flush() noexcept;
private:
	UINT width;
	UINT height;
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	bool imguiEnabled = true;
	float mFOV;
	HWND hWnd;
	//wrl::ComPtr<ID3D12DeviceContext> pContext;

	//std::shared_ptr<Bind::RenderTarget> pTarget;
	std::unique_ptr<GPUAdapter> gpuAdapter;
	std::unique_ptr<DXDevice> dxDevice;
	std::unique_ptr<CommandManager> commandManager;
	std::unique_ptr<SwapChain> swapChain;
	std::unique_ptr<DescriptorHeap> SCRTDesHeap;
	std::shared_ptr<FenceManager> fenceManager;
	HANDLE fenceEvent;
	uint64_t frameFenceValues[SwapChainBufferCount] = {};
	wrl::ComPtr<ID3D12GraphicsCommandList2> commandList;

public:
	bool isWireFrame = false;
	bool isTAA;
	bool isHBAO;
	bool isHDR;
	//bool isVolumetricRendering;
	bool isSkyRendering;
};


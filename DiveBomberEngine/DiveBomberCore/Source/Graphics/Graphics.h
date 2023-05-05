#pragma once
#include "DX\GPUAdapter.h"
#include "DX\DXDevice.h"
#include "DX\CommandQueue.h"
#include "DX\SwapChain.h"
#include "DX\DescriptorHeap.h"

namespace DiveBomber::DEGraphics
{
	using namespace DX;

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
		CommandQueue* GetCommandQueue(D3D12_COMMAND_LIST_TYPE) noexcept;
		void Flush() noexcept;
	private:
		UINT width = 0;
		UINT height = 0;
		DirectX::XMMATRIX projection = DirectX::XMMATRIX();
		DirectX::XMMATRIX camera = DirectX::XMMATRIX();
		bool imguiEnabled = true;
		float mFOV = PI / 3 * 2;
		HWND hWnd = 0;
		//wrl::ComPtr<ID3D12DeviceContext> pContext;

		//std::shared_ptr<Bind::RenderTarget> pTarget;
		std::unique_ptr<GPUAdapter> gpuAdapter;
		std::unique_ptr<DXDevice> dxDevice;
		std::unique_ptr<CommandQueue> directCommandQueue;
		std::unique_ptr<CommandQueue> computeCommandQueue;
		std::unique_ptr<CommandQueue> copyCommandQueue;
		std::unique_ptr<SwapChain> swapChain;
		std::unique_ptr<DescriptorHeap> SCRTDesHeap;
		HANDLE fenceEvent = 0;
		uint64_t frameFenceValues[SwapChainBufferCount] = {};
		wrl::ComPtr<ID3D12GraphicsCommandList2> commandList;

	public:
		bool isWireFrame = false;
		bool isTAA = true;
		bool isHBAO = true;
		bool isHDR = true;
		//bool isVolumetricRendering;
		bool isSkyRendering = true;
	};
}
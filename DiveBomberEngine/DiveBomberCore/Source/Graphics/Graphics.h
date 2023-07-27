#pragma once

#include "GraphicsHeader.h"

namespace DiveBomber::DX
{
	class GPUAdapter;
	class DXDevice;
	class CommandQueue;
	class SwapChain;
	class DescriptorHeap;
	class Viewport;
	class ScissorRects;
	class CommandList;
}

namespace DiveBomber::Component
{
	class Camera;
}

namespace DiveBomber::BindObj
{
	class DepthStencil;
}

namespace DiveBomber::DEGraphics
{
	class Graphics final
	{
	public:
		Graphics(HWND inputHWnd, UINT includeWidth, UINT includeHeight);
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		~Graphics();
		void DrawIndexed(UINT count) noxnd;
		void DrawInstanced(UINT vertexCount, UINT instanceCount) noxnd;
		void Dispatch(const UINT x, const UINT y, UINT const group) noxnd;
		void SetCamera(const std::shared_ptr<Component::Camera> camera) noexcept;
		[[nodiscard]] DirectX::XMMATRIX GetCameraMatrix() const noexcept;
		[[nodiscard]] DirectX::XMMATRIX GetProjetionMatrix() const noexcept;
		[[nodiscard]] float GetFOV() const noexcept;
		void EnableImgui() noexcept;
		void DisableImgui() noexcept;
		[[nodiscard]] bool IsImguiEnabled() const noexcept;
		[[nodiscard]] UINT GetWidth() const noexcept;
		[[nodiscard]] UINT GetHeight() const noexcept;
		//std::shared_ptr<Bind::RenderTarget> GetTarget();
		void ClearShaderResources(const UINT slot) noexcept;
		void UnbindTessellationShaders() noexcept;
		void ClearConstantBuffers(const UINT slot) noexcept;
		void ClearRenderTarget() noexcept;
		void ClearUAV(const UINT slot) noexcept;
		void ClearShader() noexcept;

		void BeginFrame();
		void EndFrame();
		[[nodiscard]] HANDLE GetFenceEvent() const noexcept;
		void ReSizeMainRT(const uint32_t inputWidth, const uint32_t inputHeight);
		[[nodiscard]] std::shared_ptr<DX::CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const noexcept;
		void Flush() const noexcept;
		[[nodiscard]] wrl::ComPtr<ID3D12Device2> GetDecive() const noexcept;
		[[nodiscard]] wrl::ComPtr<ID3D12GraphicsCommandList2> GetCommandList(const D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) noexcept;
		[[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetDescriptorHandle() const noexcept;
		[[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilDescriptorHandle() const noexcept;
		//temp
		uint64_t ExecuteCommandList();

	private:
		UINT width = 0;
		UINT height = 0;
		bool imguiEnabled = true;
		float mFOV = Utility::PI / 3 * 2;
		HWND hWnd = 0;

		std::unique_ptr<DX::GPUAdapter> gpuAdapter;
		std::unique_ptr<DX::DXDevice> dxDevice;
		std::shared_ptr<DX::CommandQueue> directCommandQueue;
		std::shared_ptr<DX::CommandQueue> computeCommandQueue;
		std::shared_ptr<DX::CommandQueue> copyCommandQueue;
		std::unique_ptr<DX::SwapChain> swapChain;
		std::unique_ptr<DX::Viewport> viewport;
		std::unique_ptr<DX::ScissorRects> scissorRects;

		HANDLE fenceEvent = 0;
		uint64_t frameFenceValues[SwapChainBufferCount] = {};

		std::shared_ptr<DX::CommandList> copyCommandList;
		std::shared_ptr<DX::CommandList> directCommandList;

		std::shared_ptr<Component::Camera> renderCamera;

		std::shared_ptr<BindObj::DepthStencil> mainDS;

	public:
		bool isWireFrame = false;
		bool isTAA = true;
		bool isHBAO = true;
		bool isHDR = true;
		//bool isVolumetricRendering;
		bool isSkyRendering = true;
	};
}
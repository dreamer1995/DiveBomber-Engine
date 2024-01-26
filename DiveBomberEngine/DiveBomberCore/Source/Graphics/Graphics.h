#pragma once

#include "GraphicsHeader.h"

namespace DiveBomber::DX
{
	class GPUAdapter;
	class DXDevice;
	class CommandQueue;
	class SwapChain;
	class CommandList;
	class DescriptorAllocator;
}

namespace DiveBomber::DEComponent
{
	class Camera;
}

namespace DiveBomber::DEResource
{
	class RenderTarget;
	class DepthStencil;
	class Viewport;
	class ScissorRects;
}

namespace DiveBomber
{
	class DiveBomberCore;
}

namespace DiveBomber::DEGraphics
{
	class Graphics final
	{
	public:
		Graphics();
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		~Graphics();

		void PostInitializeGraphics();

		void DrawIndexed(UINT count) noxnd;
		void DrawInstanced(UINT vertexCount, UINT instanceCount) noxnd;
		void Dispatch(const UINT x, const UINT y, UINT const group) noxnd;
		void SetCamera(const std::shared_ptr<DEComponent::Camera> camera) noexcept;
		[[nodiscard]] std::shared_ptr<DEComponent::Camera> GetCamera() const noexcept;
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
		[[nodiscard]] wrl::ComPtr<ID3D12Device10> GetDevice() const noexcept;
		[[nodiscard]] std::shared_ptr<DX::CommandList> GetCommandList(const D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) noexcept;
		[[nodiscard]] wrl::ComPtr<ID3D12GraphicsCommandList7> GetGraphicsCommandList(const D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) noexcept;
		[[nodiscard]] std::shared_ptr<DEResource::RenderTarget> GetCurrentBackBuffer() const noexcept;
		[[nodiscard]] std::shared_ptr<DEResource::DepthStencil> GetMainDS() const noexcept;
		uint64_t ExecuteCommandList(const D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		uint64_t ExecuteCommandList(std::shared_ptr<DX::CommandList> commandList, const D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		uint64_t ExecuteCommandLists(std::vector<std::shared_ptr<DX::CommandList>> commandLists, const D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		void ExecuteAllCurrentCommandLists();
		[[nodiscard]] std::shared_ptr<DX::DescriptorAllocator> GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) const noexcept;
		void BindShaderDescriptorHeaps();

		[[nodiscard]] static Graphics& GetInstance();
		static void Destructor() noexcept;
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

		HANDLE fenceEvent = 0;
		uint64_t frameFenceValues[SwapChainBufferCount] = {};

		std::shared_ptr<DX::CommandList> copyCommandList;
		std::shared_ptr<DX::CommandList> directCommandList;
		std::shared_ptr<DX::CommandList> computeCommandList;

		std::shared_ptr<DX::DescriptorAllocator> cbvSrvUavDescriptorHeap;
		std::shared_ptr<DX::DescriptorAllocator> rtvDescriptorHeap;
		std::shared_ptr<DX::DescriptorAllocator> dsvDescriptorHeap;
		std::shared_ptr<DX::DescriptorAllocator> samplerDescriptorHeap;

		std::shared_ptr<DEComponent::Camera> renderCamera;

		std::shared_ptr<DEResource::DepthStencil> mainDS;
		std::unique_ptr<DEResource::Viewport> viewport;
		std::unique_ptr<DEResource::ScissorRects> scissorRects;

		static std::unique_ptr<Graphics> instance;

	public:
		bool isWireFrame = false;
		bool isTAA = true;
		bool isHBAO = true;
		bool isHDR = true;
		//bool isVolumetricRendering;
		bool isSkyRendering = true;
	};
}
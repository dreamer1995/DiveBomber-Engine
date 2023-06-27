#pragma once
#include "DX\GPUAdapter.h"
#include "DX\DXDevice.h"
#include "DX\CommandQueue.h"
#include "DX\SwapChain.h"
#include "DX\DescriptorHeap.h"
#include "../Utility/RenderStatistics.h"

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
		void SetProjection(DirectX::FXMMATRIX const proj) noexcept;
		[[nodiscard]] DirectX::XMMATRIX GetProjection() const noexcept;
		void SetCamera(const DirectX::FXMMATRIX cam) noexcept;
		[[nodiscard]] DirectX::XMMATRIX GetCamera() const noexcept;
		void EnableImgui() noexcept;
		void DisableImgui() noexcept;
		[[nodiscard]] bool IsImguiEnabled() const noexcept;
		[[nodiscard]] UINT GetWidth() const noexcept;
		[[nodiscard]] UINT GetHeight() const noexcept;
		//std::shared_ptr<Bind::RenderTarget> GetTarget();
		void ClearShaderResources(const UINT slot) noexcept;
		void UnbindTessellationShaders() noexcept;
		void ClearConstantBuffers(const UINT slot) noexcept;
		void SetFOV(const float FOV) noexcept;
		[[nodiscard]] float GetFOV() const noexcept;
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
		//temp
		void OnRender();

	private:
		UINT width = 0;
		UINT height = 0;
		DirectX::XMMATRIX projection = DirectX::XMMATRIX();
		DirectX::XMMATRIX camera = DirectX::XMMATRIX();
		bool imguiEnabled = true;
		float mFOV = Utility::PI / 3 * 2;
		HWND hWnd = 0;
		//wrl::ComPtr<ID3D12DeviceContext> pContext;

		//std::shared_ptr<Bind::RenderTarget> pTarget;
		std::unique_ptr<DX::GPUAdapter> gpuAdapter;
		std::unique_ptr<DX::DXDevice> dxDevice;
		std::shared_ptr<DX::CommandQueue> directCommandQueue;
		std::shared_ptr<DX::CommandQueue> computeCommandQueue;
		std::shared_ptr<DX::CommandQueue> copyCommandQueue;
		std::unique_ptr<DX::SwapChain> swapChain;
		//std::unique_ptr<DX::DescriptorHeap> SCRTVDesHeap;
		HANDLE fenceEvent = 0;
		uint64_t frameFenceValues[SwapChainBufferCount] = {};

		//temp
	public:
		D3D12_VIEWPORT m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(MainWindowWidth), static_cast<float>(MainWindowHeight));
		D3D12_RECT m_ScissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);

		float m_FoV = 45.0f;

		DirectX::XMMATRIX m_ModelMatrix;
		DirectX::XMMATRIX m_ViewMatrix;
		DirectX::XMMATRIX m_ProjectionMatrix;

		bool m_ContentLoaded = false;

		DirectX::XMFLOAT4 eyePosition = { 0, 0, -10, 1 };
		UINT m_CurrentBackBufferIndex;

	private:
		wrl::ComPtr<ID3D12GraphicsCommandList2> copyCommandList;
		wrl::ComPtr<ID3D12GraphicsCommandList2> directCommandList;

	public:
		bool isWireFrame = false;
		bool isTAA = true;
		bool isHBAO = true;
		bool isHDR = true;
		//bool isVolumetricRendering;
		bool isSkyRendering = true;
	};
}
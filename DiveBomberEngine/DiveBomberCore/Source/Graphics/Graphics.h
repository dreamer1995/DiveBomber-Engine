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
		std::shared_ptr<DX::CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) noexcept;
		void Flush() noexcept;
		//temp
		void Load(std::vector<D3D12_INPUT_ELEMENT_DESC> vlv);
		void UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
			ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource,
			size_t numElements, size_t elementSize, const void* bufferData,
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

		void ResizeDepthBuffer(int width, int height);
		void OnRender(float time);
		wrl::ComPtr<ID3D12Device2> GetDecive() noexcept;
		wrl::ComPtr<ID3D12GraphicsCommandList2> GetCommandList(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) noexcept;
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
		std::unique_ptr<DX::DescriptorHeap> SCRTDesHeap;
		HANDLE fenceEvent = 0;
		uint64_t frameFenceValues[SwapChainBufferCount] = {};

		//temp
	public:
		uint64_t m_FenceValues[SwapChainBufferCount] = {};

		// Vertex buffer for the cube.
		Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		// Index buffer for the cube.
		Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBuffer;
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;

		// Depth buffer.
		Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthBuffer;
		// Descriptor heap for depth buffer.
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;

		// Root signature
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;

		// Pipeline state object.
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;

		D3D12_VIEWPORT m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(MainWindowWidth), static_cast<float>(MainWindowHeight));
		D3D12_RECT m_ScissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);

		float m_FoV = 45.0f;

		DirectX::XMMATRIX m_ModelMatrix;
		DirectX::XMMATRIX m_ViewMatrix;
		DirectX::XMMATRIX m_ProjectionMatrix;

		bool m_ContentLoaded = false;

		DirectX::XMFLOAT4 eyePosition = { 0, 0, -10, 1 };
		UINT m_CurrentBackBufferIndex;

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
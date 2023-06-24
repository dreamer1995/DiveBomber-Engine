#include "Graphics.h"

#include <iostream>

namespace DiveBomber::DEGraphics
{
	using namespace DX;
	using namespace DEException;

	Graphics::Graphics(HWND inputHWnd, UINT includeWidth, UINT includeHeight)
	{
		// Check for DirectX Math library support.
		if (!DirectX::XMVerifyCPUSupport())
		{
			MessageBoxA(nullptr, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		hWnd = inputHWnd;
		width = includeWidth;
		height = includeHeight;

		gpuAdapter = std::make_unique<GPUAdapter>();
		dxDevice = std::make_unique<DXDevice>(gpuAdapter->GetAdapter());

		fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		assert(fenceEvent && "Failed to create fence event.");

		directCommandQueue = std::make_unique<CommandQueue>(dxDevice->GetDecive(), D3D12_COMMAND_LIST_TYPE_DIRECT);
		computeCommandQueue = std::make_unique<CommandQueue>(dxDevice->GetDecive(), D3D12_COMMAND_LIST_TYPE_COMPUTE);
		copyCommandQueue = std::make_unique<CommandQueue>(dxDevice->GetDecive(), D3D12_COMMAND_LIST_TYPE_COPY);
		SCRTVDesHeap = std::make_unique<DescriptorHeap>(dxDevice->GetDecive(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, SwapChainBufferCount);
		swapChain = std::make_unique<SwapChain>(hWnd, directCommandQueue->GetCommandQueue());
		swapChain->UpdateMainRT(dxDevice->GetDecive(), SCRTVDesHeap->GetDescriptorHeap());
		DSVHeap = std::make_unique<DescriptorHeap>(dxDevice->GetDecive(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
	}

	Graphics::~Graphics()
	{
		Flush();
	}

	void Graphics::BeginFrame()
	{
		auto currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
		auto backBuffer = swapChain->GetBackBuffer(currentBackBufferIndex);

		auto commandQueue = GetCommandQueue();
		directCommandList = commandQueue->GetCommandList();
		wrl::ComPtr<ID3D12GraphicsCommandList2> commandList = directCommandList;

		// Clear the render target.
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffer.Get(),
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			commandList->ResourceBarrier(1, &barrier);

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(SCRTVDesHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
				currentBackBufferIndex, dxDevice->GetDecive()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

			FLOAT clearColor[] = ClearMainRTColor;
			commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		}
	}

	void Graphics::EndFrame()
	{
		auto currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
		auto backBuffer = swapChain->GetBackBuffer(currentBackBufferIndex);

		auto commandQueue = GetCommandQueue();
		wrl::ComPtr<ID3D12GraphicsCommandList2> commandList = directCommandList;

		// Present
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffer.Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			commandList->ResourceBarrier(1, &barrier);

			frameFenceValues[currentBackBufferIndex] = commandQueue->ExecuteCommandList(commandList);

			HRESULT hr;
			bool enableVSync = VSync;
			UINT syncInterval = enableVSync ? 1 : 0;
			UINT presentFlags = swapChain->CheckTearingSupport() && !enableVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
			GFX_THROW_INFO(swapChain->GetSwapChain()->Present(syncInterval, presentFlags));

			currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

			commandQueue->WaitForFenceValue(frameFenceValues[currentBackBufferIndex]);
		}
	}

	HANDLE Graphics::GetFenceEvent() const noexcept
	{
		return fenceEvent;
	}

	void Graphics::ReSizeMainRT(const uint32_t inputWidth, const uint32_t inputHeight)
	{
		// Don't allow 0 size swap chain back buffers.
		width = std::max(1u, inputWidth);
		height = std::max(1u, inputHeight);

		// Flush the GPU queue to make sure the swap chain's back buffers
		// are not being referenced by an in-flight command list.
		Flush();

		for (int i = 0; i < SwapChainBufferCount; ++i)
		{
			// Any references to the back buffers must be released
			// before the swap chain can be resized.
			swapChain->ResetBackBuffer(i);
			frameFenceValues[i] = frameFenceValues[swapChain->GetSwapChain()->GetCurrentBackBufferIndex()];
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

		HRESULT hr;
		GFX_THROW_INFO(swapChain->GetSwapChain()->GetDesc(&swapChainDesc));
		GFX_THROW_INFO(swapChain->GetSwapChain()->ResizeBuffers(SwapChainBufferCount, width, height,
			swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

		swapChain->UpdateMainRT(dxDevice->GetDecive(), SCRTVDesHeap->GetDescriptorHeap());
	}

	UINT Graphics::GetWidth() const noexcept
	{
		return width;
	}

	UINT Graphics::GetHeight() const noexcept
	{
		return height;
	}

	std::shared_ptr<CommandQueue> Graphics::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const noexcept
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			return directCommandQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return computeCommandQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return copyCommandQueue;
			break;
		default:
			assert(false && "Invalid command queue type.");
			return nullptr;
		}
	}

	void Graphics::Flush() const noexcept
	{
		directCommandQueue->Flush();
		computeCommandQueue->Flush();
		copyCommandQueue->Flush();
	}

	void Graphics::Load()
	{
		// Create the descriptor heap for the depth-stencil view.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		HRESULT hr;
		auto device = dxDevice->GetDecive();
		GFX_THROW_INFO(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));

		m_ContentLoaded = true;

		// Resize/Create the depth buffer.
		ResizeDepthBuffer(MainWindowWidth, MainWindowHeight);
	}

	void Graphics::ResizeDepthBuffer(int width, int height)
	{
		if (m_ContentLoaded)
		{
			// Flush any GPU commands that might be referencing the depth buffer.
			Flush();

			width = std::max(1, width);
			height = std::max(1, height);

			auto device = dxDevice->GetDecive();

			// Resize screen dependent resources.
			// Create a depth buffer.
			D3D12_CLEAR_VALUE optimizedClearValue = {};
			optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
			optimizedClearValue.DepthStencil = { 1.0f, 0 };

			HRESULT hr;

			auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			auto resDes = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height,
				1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

			GFX_THROW_INFO(device->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDes,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&optimizedClearValue,
				IID_PPV_ARGS(&m_DepthBuffer)
			));

			// Update the depth-stencil view.
			D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
			dsv.Format = DXGI_FORMAT_D32_FLOAT;
			dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsv.Texture2D.MipSlice = 0;
			dsv.Flags = D3D12_DSV_FLAG_NONE;

			device->CreateDepthStencilView(m_DepthBuffer.Get(), &dsv,
				m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
		}
	}

	void ClearDepth(wrl::ComPtr<ID3D12GraphicsCommandList2> commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 1.0f)
	{
		commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
	}

	void Graphics::OnRender()
	{
		wrl::ComPtr<ID3D12GraphicsCommandList2> commandList = directCommandList;

		using namespace DirectX;
		// Update the model matrix.
		float angle = (float)Utility::g_GameTime * 90.0f;
		const XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);
		m_ModelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));

		// Update the view matrix.
		const XMVECTOR eyePositionVector = XMVectorSet(eyePosition.x, eyePosition.y, eyePosition.z, eyePosition.w);
		const XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
		const XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
		m_ViewMatrix = XMMatrixLookAtLH(eyePositionVector, focusPoint, upDirection);

		// Update the projection matrix.
		float aspectRatio = MainWindowWidth / (float)MainWindowHeight;
		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_FoV), aspectRatio, 0.1f, 100.0f);


		auto currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

		auto rtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(SCRTVDesHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			currentBackBufferIndex, dxDevice->GetDecive()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		auto dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();

		// Clear the render targets.
		{
			ClearDepth(commandList, dsv);
		}

		commandList->RSSetViewports(1, &m_Viewport);
		commandList->RSSetScissorRects(1, &m_ScissorRect);

		commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		// Update the MVP matrix
		XMMATRIX mvpMatrix = XMMatrixMultiply(m_ModelMatrix, m_ViewMatrix);
		mvpMatrix = XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix);
		commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);
	}

	wrl::ComPtr<ID3D12Device2> Graphics::GetDecive() const noexcept
	{
		return dxDevice->GetDecive();
	}

	wrl::ComPtr<ID3D12GraphicsCommandList2> Graphics::GetCommandList(const D3D12_COMMAND_LIST_TYPE type) noexcept
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			if (directCommandList)
				return directCommandList;
			else
			{
				directCommandList = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandList();
				return directCommandList;
			}
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return nullptr;
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			if (copyCommandList)
				return copyCommandList;
			else
			{
				copyCommandList = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY)->GetCommandList();

				return copyCommandList;
			}
			break;
		default:
			assert(false && "Invalid command queue type.");
			return nullptr;
		}
	}
}
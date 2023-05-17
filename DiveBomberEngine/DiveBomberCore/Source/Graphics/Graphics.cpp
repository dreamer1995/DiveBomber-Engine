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
		SCRTDesHeap = std::make_unique<DescriptorHeap>(dxDevice->GetDecive(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		swapChain = std::make_unique<SwapChain>(hWnd, directCommandQueue->GetCommandQueue());
		swapChain->UpdateMainRT(dxDevice->GetDecive(), SCRTDesHeap->GetDescriptorHeap());
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
		auto commandList = directCommandList.Get();

		// Clear the render target.
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffer,
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			commandList->ResourceBarrier(1, &barrier);

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(SCRTDesHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
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
		auto commandList = directCommandList.Get();

		// Present
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffer,
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			commandList->ResourceBarrier(1, &barrier);

			m_FenceValues[currentBackBufferIndex] = commandQueue->ExecuteCommandList(commandList);

			HRESULT hr;
			bool enableVSync = VSync;
			UINT syncInterval = enableVSync ? 1 : 0;
			UINT presentFlags = swapChain->CheckTearingSupport() && !enableVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
			GFX_THROW_INFO(swapChain->GetSwapChain()->Present(syncInterval, presentFlags));

			currentBackBufferIndex = swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

			commandQueue->WaitForFenceValue(m_FenceValues[currentBackBufferIndex]);
		}
	}

	HANDLE Graphics::GetFenceEvent() noexcept
	{
		return fenceEvent;
	}

	void Graphics::ReSizeMainRT(uint32_t inputWidth, uint32_t inputHeight)
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

		swapChain->UpdateMainRT(dxDevice->GetDecive(), SCRTDesHeap->GetDescriptorHeap());
	}

	UINT Graphics::GetWidth() const noexcept
	{
		return width;
	}

	UINT Graphics::GetHeight() const noexcept
	{
		return height;
	}

	CommandQueue* Graphics::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) noexcept
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			return directCommandQueue.get();
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return computeCommandQueue.get();
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return copyCommandQueue.get();
			break;
		default:
			assert(false && "Invalid command queue type.");
			return nullptr;
		}
	}

	void Graphics::Flush() noexcept
	{
		directCommandQueue->Flush();
		computeCommandQueue->Flush();
		copyCommandQueue->Flush();
	}

	void Graphics::UpdateBufferResource(
		wrl::ComPtr<ID3D12GraphicsCommandList2> commandList,
		ID3D12Resource** pDestinationResource,
		ID3D12Resource** pIntermediateResource,
		size_t numElements, size_t elementSize, const void* bufferData,
		D3D12_RESOURCE_FLAGS flags)
	{
		auto device = dxDevice->GetDecive();

		size_t bufferSize = numElements * elementSize;

		HRESULT hr;
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resDes = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);

		// Create a committed resource for the GPU resource in a default heap.
		GFX_THROW_INFO(device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDes,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(pDestinationResource)));

		// Create an committed resource for the upload.
		if (bufferData)
		{
			auto heapProp1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto resDes1 = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

			GFX_THROW_INFO(device->CreateCommittedResource(
				&heapProp1,
				D3D12_HEAP_FLAG_NONE,
				&resDes1,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(pIntermediateResource)));

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = bufferData;
			subresourceData.RowPitch = bufferSize;
			subresourceData.SlicePitch = subresourceData.RowPitch;

			UpdateSubresources(commandList.Get(),
				*pDestinationResource, *pIntermediateResource,
				0, 0, 1, &subresourceData);
		}
	}

	// Vertex data for a colored cube.
	struct VertexPosColor
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Color;
	};

	static VertexPosColor g_Vertices[8] = {
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
		{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
		{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
		{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
		{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
		{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
		{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
	};

	static WORD g_Indicies[36] =
	{
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		4, 5, 1, 4, 1, 0,
		3, 2, 6, 3, 6, 7,
		1, 5, 6, 1, 6, 2,
		4, 0, 3, 4, 3, 7
	};

	void Graphics::Load(std::vector<D3D12_INPUT_ELEMENT_DESC> vlv)
	{
		//auto commandQueue = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		//copyCommandList = commandQueue->GetCommandList();
		//auto commandList = copyCommandList.Get();

		//// Upload vertex buffer data.
		//wrl::ComPtr<ID3D12Resource> intermediateVertexBuffer;
		//UpdateBufferResource(commandList,
		//	&m_VertexBuffer, &intermediateVertexBuffer,
		//	_countof(g_Vertices), sizeof(VertexPosColor), g_Vertices);

		//// Create the vertex buffer view.
		//m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		//m_VertexBufferView.SizeInBytes = sizeof(g_Vertices);
		//m_VertexBufferView.StrideInBytes = sizeof(VertexPosColor);

		//// Upload index buffer data.
		//wrl::ComPtr<ID3D12Resource> intermediateIndexBuffer;
		//UpdateBufferResource(commandList,
		//	&m_IndexBuffer, &intermediateIndexBuffer,
		//	_countof(g_Indicies), sizeof(WORD), g_Indicies);

		//// Create index buffer view.
		//m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		//m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
		//m_IndexBufferView.SizeInBytes = sizeof(g_Indicies);

		// Create the descriptor heap for the depth-stencil view.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		HRESULT hr;
		auto device = dxDevice->GetDecive();
		GFX_THROW_INFO(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));

		// Load the vertex shader.
		wrl::ComPtr<ID3DBlob> vertexShaderBlob;
		GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob));

		// Load the pixel shader.
		wrl::ComPtr<ID3DBlob> pixelShaderBlob;
		GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob));

		//// Create the vertex input layout
		//D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//};

		D3D12_INPUT_ELEMENT_DESC inputLayout[5];
		for (int i = 0; i < 5; i++)
		{
			inputLayout[i] = vlv[i];
		}

		// Create a root signature.
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Allow input layout and deny unnecessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		// A single 32-bit constant root parameter that is used by the vertex shader.
		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

		// Serialize the root signature.
		wrl::ComPtr<ID3DBlob> rootSignatureBlob;
		wrl::ComPtr<ID3DBlob> errorBlob;
		GFX_THROW_INFO(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
			featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
		// Create the root signature.
		GFX_THROW_INFO(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));

		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		pipelineStateStream.pRootSignature = m_RootSignature.Get();
		pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		GFX_THROW_INFO(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));

		//auto fenceValue = commandQueue->ExecuteCommandList(commandList);
		//commandQueue->WaitForFenceValue(fenceValue);

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

	void ClearDepth(ID3D12GraphicsCommandList2* commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 1.0f)
	{
		commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
	}

	void Graphics::OnRender(float time)
	{
		auto commandList = directCommandList.Get();

		using namespace DirectX;
		// Update the model matrix.
		float angle = time * 90.0f;
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
		auto backBuffer = swapChain->GetBackBuffer(currentBackBufferIndex);

		auto rtv = CD3DX12_CPU_DESCRIPTOR_HANDLE(SCRTDesHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			currentBackBufferIndex, dxDevice->GetDecive()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		auto dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();

		// Clear the render targets.
		{
			ClearDepth(commandList, dsv);
		}

		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//commandList->IASetVertexBuffers(0u, 1u, &m_VertexBufferView);
		//commandList->IASetIndexBuffer(&m_IndexBufferView);

		commandList->RSSetViewports(1, &m_Viewport);
		commandList->RSSetScissorRects(1, &m_ScissorRect);

		commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		// Update the MVP matrix
		XMMATRIX mvpMatrix = XMMatrixMultiply(m_ModelMatrix, m_ViewMatrix);
		mvpMatrix = XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix);
		commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);

		//commandList->DrawIndexedInstanced(_countof(g_Indicies), 1, 0, 0, 0);
	}

	ID3D12Device2* Graphics::GetDecive() noexcept
	{
		return dxDevice->GetDecive();
	}

	ID3D12GraphicsCommandList2* Graphics::GetCommandList(D3D12_COMMAND_LIST_TYPE type) noexcept
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			if (directCommandList)
				return directCommandList.Get();
			else
			{
				directCommandList = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandList();
				return directCommandList.Get();
			}
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return nullptr;
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			if (copyCommandList)
				return copyCommandList.Get();
			else
			{
				copyCommandList = GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandList();

				return copyCommandList.Get();
			}
			break;
		default:
			assert(false && "Invalid command queue type.");
			return nullptr;
		}
	}
}
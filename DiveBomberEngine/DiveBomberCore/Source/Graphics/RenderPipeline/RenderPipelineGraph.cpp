#include "RenderPipelineGraph.h"

#include "..\Graphics.h"
#include "..\BindObj\BindObjCommon.h"
#include "..\Gizmo\Geometry\Sphere.h"
#include "..\Component\Camera.h"
#include "..\DX\CommandQueue.h"
#include "..\..\Utility\GlobalParameters.h"
#include "..\BindObj\Texture.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"

#include <iostream>

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DiveBomber::BindObj;
	using namespace DiveBomber::BindObj::VertexProcess;
	using namespace DX;
	using namespace Component;

	RenderPipelineGraph::RenderPipelineGraph()
	{
	}

	void RenderPipelineGraph::LoadContent(Graphics& gfx)
	{
		VertexLayout vl;
		vl.Append(VertexLayout::Position3D);
		vl.Append(VertexLayout::Normal);
		vl.Append(VertexLayout::Tangent);
		vl.Append(VertexLayout::Binormal);
		vl.Append(VertexLayout::Texture2D);
		mesh = std::make_shared<BindObj::IndexedTriangleList>(Sphere::MakeNormalUVed(vl, true));
		mesh->Transform(dx::XMMatrixScaling(1, 1, 1));
		const auto geometryTag = "$sphere." + std::to_string(1);

		auto commandQueue = gfx.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

		vertexBuffer = std::make_shared<VertexBuffer>(gfx, geometryTag, mesh->vertices);
		indexBuffer = std::make_shared<IndexBuffer>(gfx, geometryTag, mesh->indices);

		std::shared_ptr<DescriptorAllocation> descriptorAllocation =
			gfx.GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Allocate(1u);
		texture = std::make_shared<Texture>(gfx, L"Asset\\Texture\\earth.jpg", std::move(descriptorAllocation));

		descriptorAllocation =
			gfx.GetDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Allocate(1u);
		Texture::TextureDescription textureDesc;
		textureDesc.generateMip = true;
		texture2 = std::make_shared<Texture>(gfx, L"Asset\\Texture\\rustediron2_basecolor.png", std::move(descriptorAllocation), std::move(textureDesc));

		auto fenceValue = gfx.ExecuteCommandList(D3D12_COMMAND_LIST_TYPE_COPY);
		commandQueue->WaitForFenceValue(fenceValue);

		vertexShader = std::make_shared<Shader>(gfx, L"VShader.cso", Shader::ShaderType::VertexShader);
		pixelShader = std::make_shared<Shader>(gfx, L"PShader.cso", Shader::ShaderType::PixelShader);

		topology = std::make_shared<Topology>(gfx, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		rootSignature = std::make_shared<RootSignature>(gfx, "sphere1RootSignature");

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		auto dsvFormat = DXGI_FORMAT_D32_FLOAT;

		pipelineStateObject = std::make_shared<PipelineStateObject>(gfx, "sphere1PSO",
			rootSignature, vertexBuffer, topology, vertexShader, pixelShader, dsvFormat, rtvFormats);

		Camera::CameraAttributes cameraAttr;
		cameraAttr.position.z = -6.0f;
		mainCamera = std::make_shared<Camera>(gfx, "Main Camera", cameraAttr, false);
		mainCamera->BindToGraphics(gfx, mainCamera);

		transformCBuffer = std::make_shared<ConstantTransformBuffer>(gfx);
	}

	void RenderPipelineGraph::Bind(DEGraphics::Graphics& gfx) noxnd
	{
		gfx.BindShaderDescriptorHeaps();

		rootSignature->Bind(gfx);
		pipelineStateObject->Bind(gfx);

		vertexBuffer->Bind(gfx);
		indexBuffer->Bind(gfx);

		topology->Bind(gfx);

		using namespace DirectX;
		// Update the model matrix.
		float angle = (float)Utility::g_GameTime * 90.0f;
		const XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);
		auto modelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));

		transformCBuffer->InitializeParentReference(modelMatrix);
		transformCBuffer->Bind(gfx);

		gfx.GetCurrentBackBuffer()->BindTarget(gfx, gfx.GetMainDS());

		gfx.GetGraphicsCommandList()->DrawIndexedInstanced(indexBuffer->GetCount(), 1, 0, 0, 0);
	}

	std::shared_ptr<Component::Camera> RenderPipelineGraph::GetMainCamera() const noexcept
	{
		return mainCamera;
	}
}

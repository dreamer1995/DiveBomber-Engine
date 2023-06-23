#include "RenderPipelineGraph.h"

#include <iostream>

namespace DiveBomber::RenderPipeline
{
	using namespace DEGraphics;
	using namespace DiveBomber::BindObj;
	using namespace DiveBomber::BindObj::VertexProcess;

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
		auto commandList = gfx.GetCommandList(D3D12_COMMAND_LIST_TYPE_COPY);

		vertexBuffer = std::make_shared<VertexBuffer>(gfx, geometryTag, mesh->vertices);
		indexBuffer = std::make_shared<IndexBuffer>(gfx, geometryTag, mesh->indices);

		auto fenceValue = commandQueue->ExecuteCommandList(commandList);
		commandQueue->WaitForFenceValue(fenceValue);

		vertexShader = std::make_shared<Shader>(gfx, L"VertexShader.cso", Shader::ShaderType::VertexShader);
		pixelShader = std::make_shared<Shader>(gfx, L"PixelShader.cso", Shader::ShaderType::PixelShader);

		topology = std::make_shared<Topology>(gfx, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		rootSignature = std::make_shared<RootSignature>(gfx, "sphere1RootSignature");

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		auto dsvFormat = DXGI_FORMAT_D32_FLOAT;

		pipelineStateObject = std::make_shared<PipelineStateObject>(gfx, "sphere1PSO",
			rootSignature, vertexBuffer, topology, vertexShader, pixelShader, dsvFormat, rtvFormats);

		gfx.Load();
	}

	void RenderPipelineGraph::Bind(DEGraphics::Graphics& gfx) noxnd
	{
		rootSignature->Bind(gfx);
		pipelineStateObject->Bind(gfx);

		vertexBuffer->Bind(gfx);
		indexBuffer->Bind(gfx);

		topology->Bind(gfx);

		//why global time cant work???????????
		gfx.OnRender();

		gfx.GetCommandList()->DrawIndexedInstanced(indexBuffer->GetCount(), 1, 0, 0, 0);
	}
}

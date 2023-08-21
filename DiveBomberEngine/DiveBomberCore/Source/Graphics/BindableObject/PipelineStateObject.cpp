#include "PipelineStateObject.h"

#include "..\..\DiveBomberCore.h"
#include "..\Graphics.h"
#include "..\..\Exception\GraphicsException.h"
#include "RootSignature.h"
#include "VertexBuffer.h"
#include "Topology.h"
#include "Shader.h"

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;

	PipelineStateObject::PipelineStateObject(Graphics& gfx, const std::string& inputTag,
		std::shared_ptr<RootSignature> rootSignature, std::shared_ptr<VertexBuffer> vertexBuffer,
		std::shared_ptr<Topology> topology,
		std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> pixelShader,
		DXGI_FORMAT dsvFormat, D3D12_RT_FORMAT_ARRAY rtvFormats)
		:
		tag(inputTag)
	{
		HRESULT hr;

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = vertexBuffer->GetLayout().GetD3DLayout();

		pipelineStateStream.pRootSignature = rootSignature->GetRootSignature().Get();
		pipelineStateStream.InputLayout = { &inputLayout[0], (UINT)inputLayout.size() };
		pipelineStateStream.PrimitiveTopologyType = topology->GetShaderTopology();
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShader->GetBytecode().Get());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShader->GetBytecode().Get());
		pipelineStateStream.DSVFormat = dsvFormat;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};

		GFX_THROW_INFO(gfx.GetDecive()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)));
	}

	wrl::ComPtr<ID3D12PipelineState> PipelineStateObject::GetPipelineStateObject() noexcept
	{
		return pipelineState;
	}

	void PipelineStateObject::Bind(Graphics& gfx) noxnd
	{
		GFX_THROW_INFO_ONLY(gfx.GetGraphicsCommandList()->SetPipelineState(pipelineState.Get()));
	}

	std::shared_ptr<PipelineStateObject> PipelineStateObject::Resolve(Graphics& gfx, const std::string& tag,
		const std::shared_ptr<RootSignature> rootSignature, const std::shared_ptr<VertexBuffer> vertexBuffer,
		const std::shared_ptr<Topology> topology,
		const std::shared_ptr<Shader> vertexShader, const std::shared_ptr<Shader> pixelShader,
		const DXGI_FORMAT dsvFormat, const D3D12_RT_FORMAT_ARRAY rtvFormats)
	{
		return gfx.GetParent().Resolve<PipelineStateObject>(gfx, tag,
			rootSignature, vertexBuffer, topology, vertexShader, pixelShader, dsvFormat, rtvFormats);
	}

	std::string PipelineStateObject::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(PipelineStateObject).name() + "#"s + tag;
	}

	std::string PipelineStateObject::GetUID() const noexcept
	{
		return GenerateUID(tag);
	}
}
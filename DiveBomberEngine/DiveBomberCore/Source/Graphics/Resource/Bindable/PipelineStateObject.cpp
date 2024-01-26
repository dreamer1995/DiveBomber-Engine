#include "PipelineStateObject.h"

#include "..\..\GraphicsSource.h"
#include "RootSignature.h"
#include "IndexBuffer.h"
#include "Topology.h"
#include "..\Shader.h"
#include "..\..\DX\ShaderManager.h"
#include "..\..\..\Component\Mesh.h"
#include "..\..\..\Component\Material.h"

namespace DiveBomber::DEResource
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;
	using namespace DEComponent;

	PipelineStateObject::PipelineStateObject(const std::wstring& inputName, PipelineStateReference inputPipelineStateReference)
		:
		Resource(inputName),
		pipelineStateReference(inputPipelineStateReference)
	{
		UpdatePipelineState();
	}

	PipelineStateObject::PipelineStateReference PipelineStateObject::GetPipelineStateReference() const noexcept
	{
		return pipelineStateReference;
	}

	wrl::ComPtr<ID3D12PipelineState> PipelineStateObject::GetPipelineStateObject() const noexcept
	{
		return pipelineState;
	}

	void PipelineStateObject::Bind() noxnd
	{
		if (pipelineStateReference.material->IsShaderDirty())
		{
			UpdatePipelineState();
		}

		GFX_THROW_INFO_ONLY(Graphics::GetInstance().GetGraphicsCommandList()->SetPipelineState(pipelineState.Get()));
	}

	void PipelineStateObject::AssignShader(PipelineStateStream& pipelineStateStream) noexcept
	{
		for (std::shared_ptr<Shader>& shader : pipelineStateReference.material->GetShaders())
		{
			if (shader)
			{
				switch (shader->GetShaderType())
				{
				case ShaderType::VertexShader:
					pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(shader->GetBytecode().Get());
					break;
				case ShaderType::HullShader:
					pipelineStateStream.HS = CD3DX12_SHADER_BYTECODE(shader->GetBytecode().Get());
					break;
				case ShaderType::DomainShader:
					pipelineStateStream.DS = CD3DX12_SHADER_BYTECODE(shader->GetBytecode().Get());
					break;
				case ShaderType::GeometryShader:
					pipelineStateStream.GS = CD3DX12_SHADER_BYTECODE(shader->GetBytecode().Get());
					break;
				case ShaderType::PixelShader:
					pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(shader->GetBytecode().Get());
					break;
				case ShaderType::ComputeShader:
					pipelineStateStream.CS = CD3DX12_SHADER_BYTECODE(shader->GetBytecode().Get());
					break;
				}
			}
		}
	}

	std::string PipelineStateObject::GetUID() const noexcept
	{
		return GenerateUID(name);
	}

	void DiveBomber::DEResource::PipelineStateObject::UpdatePipelineState(const PipelineStateReference inputPipelineStateReference)
	{
		pipelineStateReference = inputPipelineStateReference;
		UpdatePipelineState();
	}

	void DiveBomber::DEResource::PipelineStateObject::UpdatePipelineState()
	{
		HRESULT hr;

		//std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = pipelineStateReference.mesh->GetVertexBuffer()->GetLayout().GetD3DLayout();

		PipelineStateStream pipelineStateStream;

		pipelineStateStream.pRootSignature = pipelineStateReference.rootSignature->GetRootSignature().Get();
		//pipelineStateStream.InputLayout = { &inputLayout[0], (UINT)inputLayout.size() };
		pipelineStateStream.PrimitiveTopologyType = pipelineStateReference.mesh ? pipelineStateReference.mesh->GetTopology()->GetShaderTopology() : D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		AssignShader(pipelineStateStream);
		pipelineStateStream.DSVFormat = pipelineStateReference.dsvFormat;
		pipelineStateStream.RTVFormats = pipelineStateReference.rtvFormats;
		pipelineStateStream.rasterizer = pipelineStateReference.rasterizerDesc;
		pipelineStateStream.depthStencil = pipelineStateReference.depthStencilDesc;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};

		GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)));
	}
}
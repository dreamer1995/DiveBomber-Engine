#include "PipelineStateObject.h"

#include "..\..\DiveBomberCore.h"
#include "..\Graphics.h"
#include "GlobalBindableManager.h"
#include "..\..\Exception\GraphicsException.h"
#include "RootSignature.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Topology.h"
#include "Shader.h"
#include "..\DX\ShaderManager.h"
#include "..\Component\Mesh.h"
#include "..\Component\Material.h"

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;
	using namespace DX;
	using namespace Component;

	PipelineStateObject::PipelineStateObject(const std::string& inputTag, PipelineStateReference inputPipelineStateReference)
		:
		tag(inputTag),
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
		pipelineStateReference.mesh->Bind();
		pipelineStateReference.rootSignature->Bind();
		pipelineStateReference.material->Bind();

		if (IsShaderDirty())
		{
			UpdatePipelineState();
		}

		GFX_THROW_INFO_ONLY(Graphics::GetInstance().GetGraphicsCommandList()->SetPipelineState(pipelineState.Get()));

		Graphics::GetInstance().GetGraphicsCommandList()->DrawIndexedInstanced(pipelineStateReference.mesh->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}

	std::shared_ptr<PipelineStateObject> PipelineStateObject::Resolve(const std::string& tag, PipelineStateReference pipelineStateReference)
	{
		return GlobalBindableManager::Resolve<PipelineStateObject>(tag, pipelineStateReference);
	}

	std::string PipelineStateObject::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(PipelineStateObject).name() + "#"s + tag;
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
		return GenerateUID(tag);
	}

	bool DiveBomber::BindableObject::PipelineStateObject::IsShaderDirty() noexcept
	{
		bool isShaderDirty = false;
		for (std::shared_ptr<Shader>& shader : pipelineStateReference.material->GetShaders())
		{
			if (shader)
			{
				isShaderDirty |= shader->IsDirty();
			}
		}

		return isShaderDirty;
	}

	void DiveBomber::BindableObject::PipelineStateObject::UpdatePipelineState(const PipelineStateReference inputPipelineStateReference)
	{
		pipelineStateReference = inputPipelineStateReference;
		UpdatePipelineState();
	}

	void DiveBomber::BindableObject::PipelineStateObject::UpdatePipelineState()
	{
		HRESULT hr;

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = pipelineStateReference.mesh->GetVertexBuffer()->GetLayout().GetD3DLayout();

		PipelineStateStream pipelineStateStream;

		pipelineStateStream.pRootSignature = pipelineStateReference.rootSignature->GetRootSignature().Get();
		pipelineStateStream.InputLayout = { &inputLayout[0], (UINT)inputLayout.size() };
		pipelineStateStream.PrimitiveTopologyType = pipelineStateReference.mesh->GetTopology()->GetShaderTopology();
		AssignShader(pipelineStateStream);
		pipelineStateStream.DSVFormat = pipelineStateReference.dsvFormat;
		pipelineStateStream.RTVFormats = pipelineStateReference.rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};

		GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)));
	}
}
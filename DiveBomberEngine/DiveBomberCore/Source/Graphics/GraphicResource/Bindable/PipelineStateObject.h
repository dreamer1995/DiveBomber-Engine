#pragma once
#include "..\..\..\Resource.h"
#include "Bindable.h"
#include "..\..\GraphicsHeader.h"

namespace DiveBomber::DEComponent
{
	class Mesh;
	class Material;
}

namespace DiveBomber::GraphicResource
{
	class RootSignature;
	class VertexBuffer;
	class Topology;
	class Shader;

	class PipelineStateObject final : public DiveBomber::Resource, public Bindable
	{
	public:
		struct PipelineStateReference
		{
		public:
			std::shared_ptr<RootSignature> rootSignature;
			std::shared_ptr<DEComponent::Mesh> mesh;
			std::shared_ptr<DEComponent::Material> material;
			DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;
			D3D12_RT_FORMAT_ARRAY rtvFormats{};
			CD3DX12_RASTERIZER_DESC rasterizerDesc{ D3D12_DEFAULT };
			CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc{ D3D12_DEFAULT };
		};

	public:
		PipelineStateObject(const std::wstring& inputTag, PipelineStateReference inputPipelineStateReference);

		[[nodiscard]] PipelineStateReference GetPipelineStateReference() const noexcept;
		[[nodiscard]] wrl::ComPtr<ID3D12PipelineState> GetPipelineStateObject() const noexcept;
		void Bind() noxnd override;
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::wstring& name, Ignore&&...ignore)
		{
			using namespace std::string_literals;
			return typeid(PipelineStateObject).name() + "#"s + Utility::ToNarrow(name);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;
		void UpdatePipelineState(const PipelineStateReference inputPipelineStateReference);
		void UpdatePipelineState();

	private:
		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_HS HS;
			CD3DX12_PIPELINE_STATE_STREAM_DS DS;
			CD3DX12_PIPELINE_STATE_STREAM_GS GS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_CS CS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
			CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER rasterizer;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL depthStencil;
		};

	private:
		void AssignShader(PipelineStateStream& pipelineStateStream) noexcept;

	private:
		wrl::ComPtr<ID3D12PipelineState> pipelineState;
		PipelineStateReference pipelineStateReference;
	};
}
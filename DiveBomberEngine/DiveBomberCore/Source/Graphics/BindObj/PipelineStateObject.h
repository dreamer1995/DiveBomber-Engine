#pragma once
#include "Bindable.h"
#include "..\Graphics.h"
#include "RootSignature.h"
#include "VertexBuffer.h"
#include "Topology.h"
#include "Shader.h"

namespace DiveBomber::BindObj
{
	class PipelineStateObject final : public Bindable
	{
	public:
		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		};
	public:
		PipelineStateObject(DEGraphics::Graphics& gfx, const std::string& inputTag,
			std::shared_ptr<RootSignature> rootSignature, std::shared_ptr<VertexBuffer> vertexBuffer,
			std::shared_ptr<Topology> topology,
			std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> pixelShader,
			DXGI_FORMAT dsvFormat, D3D12_RT_FORMAT_ARRAY rtvFormats);
		[[nodiscard]] wrl::ComPtr<ID3D12PipelineState> GetPipelineStateObject() noexcept;
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		[[nodiscard]] static std::shared_ptr<PipelineStateObject> Resolve(DEGraphics::Graphics& gfx, const std::string& tag,
			const std::shared_ptr<RootSignature> rootSignature, const std::shared_ptr<VertexBuffer> vertexBuffer,
			const std::shared_ptr<Topology> topology,
			const std::shared_ptr<Shader> vertexShader, const std::shared_ptr<Shader> pixelShader,
			const DXGI_FORMAT dsvFormat, const D3D12_RT_FORMAT_ARRAY rtvFormats);
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		[[nodiscard]] static std::string GenerateUID_(const std::string& tag);
		std::string tag;
		wrl::ComPtr<ID3D12PipelineState> pipelineState;
		PipelineStateStream pipelineStateStream;
		std::shared_ptr<RootSignature> rootSignature;
		std::shared_ptr<VertexBuffer> vertexBuffer;
		std::shared_ptr<Topology> topology;
		std::shared_ptr<Shader> vertexShader;
		std::shared_ptr<Shader> pixelShader;
		DXGI_FORMAT dsvFormat;
		D3D12_RT_FORMAT_ARRAY rtvFormats;
	};
}
#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::BindableObject
{
	class RootSignature;
	class VertexBuffer;
	class Topology;
	class Shader;

	struct PipelineStateShader
	{
		std::shared_ptr<Shader> vertexShader;
		std::shared_ptr<Shader> hullShader;
		std::shared_ptr<Shader> domainShader;
		std::shared_ptr<Shader> geometryShader;
		std::shared_ptr<Shader> pixelShader;
		std::shared_ptr<Shader> computeShader;
	};

	class PipelineStateObject final : public Bindable
	{
	public:
		struct PipelineStateReference
		{
			std::shared_ptr<RootSignature> rootSignature;
			std::shared_ptr<VertexBuffer> vertexBuffer;
			std::shared_ptr<Topology> topology;
			PipelineStateShader pipelineStateShader;
			DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;
			D3D12_RT_FORMAT_ARRAY rtvFormats{};
		};

	public:
		PipelineStateObject(const std::string& inputTag, PipelineStateReference inputPipelineStateReference);

		[[nodiscard]] PipelineStateReference GetPipelineStateReference() const noexcept;
		[[nodiscard]] wrl::ComPtr<ID3D12PipelineState> GetPipelineStateObject() const noexcept;
		void Bind() noxnd override;
		[[nodiscard]] static std::shared_ptr<PipelineStateObject> Resolve(const std::string& tag, PipelineStateReference pipelineStateReference);
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;
		[[nodiscard]] bool IsShaderDirty() noexcept;
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
		};

	private:
		[[nodiscard]] static std::string GenerateUID_(const std::string& tag);
		template<typename T>
		void AssignShader(T& pipelineStateStreamData, ID3DBlob* blob)
		{
			if (blob)
			{
				pipelineStateStreamData = CD3DX12_SHADER_BYTECODE(blob);
			}
		}

	private:
		std::string tag;
		wrl::ComPtr<ID3D12PipelineState> pipelineState;
		PipelineStateReference pipelineStateReference;
	};
}
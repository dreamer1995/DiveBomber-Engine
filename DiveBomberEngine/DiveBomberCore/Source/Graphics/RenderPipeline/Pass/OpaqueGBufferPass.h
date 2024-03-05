#pragma once
#include "RenderPass.h"
#include "..\..\GraphicsHeader.h"

namespace DiveBomber::GraphicResource
{
	class RenderTargetAsShaderResourceView;
}

namespace DiveBomber::RenderPipeline
{
	class OpaqueGBufferPass final : public RenderPass
	{
	public:
		OpaqueGBufferPass(std::shared_ptr<GraphicResource::RenderTarget> inputRenderTarget,
			std::shared_ptr<GraphicResource::DepthStencil> inputDepthStencil);

		void Execute() noxnd override;

	private:
		std::shared_ptr<GraphicResource::RenderTargetAsShaderResourceView> baseColorBuffer;
		std::shared_ptr<GraphicResource::RenderTargetAsShaderResourceView> roughAOShadowSMIDBuffer;
		std::shared_ptr<GraphicResource::RenderTargetAsShaderResourceView> normalBuffer;
		std::shared_ptr<GraphicResource::RenderTargetAsShaderResourceView> customDataBuffer;

		std::vector<std::shared_ptr<GraphicResource::RenderTarget>> GBufferSet;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> targetHandles;
	};
}
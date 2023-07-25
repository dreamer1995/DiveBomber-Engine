#pragma once
#include "..\..\Utility\Common.h"

#include <memory>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::Component
{
	class Camera;
}

namespace DiveBomber::BindObj
{
	class RootSignature;
	class PipelineStateObject;
	class VertexBuffer;
	class IndexBuffer;
	class Topology;
	class Shader;
	class ConstantTransformBuffer;
	class IndexedTriangleList;
}

namespace DiveBomber::RenderPipeline
{
	class RenderPipelineGraph
	{
	public:
		RenderPipelineGraph();
		void LoadContent(DEGraphics::Graphics& gfx);
		void Bind(DEGraphics::Graphics& gfx) noxnd;
		std::shared_ptr<Component::Camera> GetMainCamera() const noexcept;
	private:
		std::shared_ptr<BindObj::RootSignature> rootSignature;
		std::shared_ptr<BindObj::PipelineStateObject> pipelineStateObject;

		std::shared_ptr<BindObj::Shader> vertexShader;
		std::shared_ptr<BindObj::Shader> pixelShader;

		std::shared_ptr<BindObj::IndexedTriangleList> mesh;
		std::shared_ptr<BindObj::VertexBuffer> vertexBuffer;
		std::shared_ptr<BindObj::IndexBuffer> indexBuffer;
		std::shared_ptr<BindObj::Topology> topology;

		std::shared_ptr<Component::Camera> mainCamera;

		std::shared_ptr<BindObj::ConstantTransformBuffer> transformCBuffer;
	};
}
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

namespace DiveBomber::BindableObject
{
	class RootSignature;
	class PipelineStateObject;
	class VertexBuffer;
	class IndexBuffer;
	class Topology;
	class Shader;
	class ConstantTransformBuffer;
	class IndexedTriangleList;
	class Texture;
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
		std::shared_ptr<BindableObject::RootSignature> rootSignature;
		std::shared_ptr<BindableObject::PipelineStateObject> pipelineStateObject;

		std::shared_ptr<BindableObject::Shader> vertexShader;
		std::shared_ptr<BindableObject::Shader> pixelShader;

		std::shared_ptr<BindableObject::IndexedTriangleList> mesh;
		std::shared_ptr<BindableObject::VertexBuffer> vertexBuffer;
		std::shared_ptr<BindableObject::IndexBuffer> indexBuffer;
		std::shared_ptr<BindableObject::Topology> topology;

		std::shared_ptr<Component::Camera> mainCamera;

		std::shared_ptr<BindableObject::ConstantTransformBuffer> transformCBuffer;

		std::shared_ptr<BindableObject::Texture> texture;
		std::shared_ptr<BindableObject::Texture> texture2;
	};
}
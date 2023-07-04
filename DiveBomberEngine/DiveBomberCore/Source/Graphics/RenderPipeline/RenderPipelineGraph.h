#pragma once
#include "..\Graphics.h"
#include "..\BindObj\BindObjCommon.h"
#include "..\Gizmo\Geometry\Sphere.h"
#include "..\Component\Camera.h"

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

		std::shared_ptr<BindObj::DepthStencil> mainDS;

		std::shared_ptr<Component::Camera> mainCamera;

		std::shared_ptr<BindObj::ConstantTransformBuffer> transformCBuffer;
	};
}
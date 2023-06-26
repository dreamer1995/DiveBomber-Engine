#pragma once
#include "..\Graphics.h"
#include "..\BindObj\BindObjCommon.h"
#include "..\BindObj\Geometry\Sphere.h"

namespace DiveBomber::RenderPipeline
{
	class RenderPipelineGraph
	{
	public:
		RenderPipelineGraph();
		void LoadContent(DEGraphics::Graphics& gfx);
		void Bind(DEGraphics::Graphics& gfx) noxnd;
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
	};
}
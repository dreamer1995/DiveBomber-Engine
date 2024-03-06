#pragma once
#include "Component.h"
#include "..\Graphics\GraphicsHeader.h"
#include "..\Graphics\Geometry\Vertex.h"

namespace DiveBomber::GraphicResource
{
	class IndexBuffer;
	class Topology;
	template<typename C>
	class ConstantBufferInRootSignature;
	template<typename C>
	class StructuredBufferInHeap;
}

namespace DiveBomber::GraphicResource::VertexProcess
{
	class VertexData;
}

namespace DiveBomber::DEComponent
{
	class Mesh final : public Component
	{
	public:
		Mesh(std::wstring inputName,
			GraphicResource::VertexProcess::VertexData& inputVertexbuffer, std::shared_ptr<GraphicResource::IndexBuffer> inputIndexBuffer);
		Mesh(std::wstring inputName,
			GraphicResource::VertexProcess::VertexData& inputVertexbuffer, std::shared_ptr<GraphicResource::IndexBuffer> inputIndexBuffer,
			std::shared_ptr<GraphicResource::Topology> inputTopology);

		void SetMesh(GraphicResource::VertexProcess::VertexData& inputVertexbuffer,
			std::shared_ptr<GraphicResource::IndexBuffer> inputIndexBuffer) noexcept;
		void SetTopology(std::shared_ptr<GraphicResource::Topology> inputTopology) noexcept;
		[[nodiscard]] GraphicResource::VertexProcess::VertexData& GetVertexData() noexcept;
		[[nodiscard]] std::shared_ptr<GraphicResource::IndexBuffer> GetIndexBuffer() const noexcept;
		[[nodiscard]] std::shared_ptr<GraphicResource::Topology> GetTopology() const noexcept;

		void Bind() noxnd;

		void DrawComponentUI() override;
	private:
		GraphicResource::VertexProcess::VertexData vertexData;
		std::shared_ptr<GraphicResource::IndexBuffer> indexBuffer;
		std::shared_ptr<GraphicResource::Topology> topology;

		std::shared_ptr<GraphicResource::ConstantBufferInRootSignature<UINT>> vertexDataIndexCB;
		std::shared_ptr<GraphicResource::StructuredBufferInHeap<char>> vertexBuffer;
	};
}
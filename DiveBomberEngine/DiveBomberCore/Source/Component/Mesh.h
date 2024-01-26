#pragma once
#include "Component.h"
#include "..\Graphics\GraphicsHeader.h"
#include "..\Graphics\Geometry\Vertex.h"

namespace DiveBomber::DEResource
{
	class IndexBuffer;
	class Topology;
	template<typename C>
	class ConstantBufferInRootSignature;
	template<typename C>
	class StructuredBufferInHeap;
}

namespace DiveBomber::DEResource::VertexProcess
{
	class VertexData;
}

namespace DiveBomber::DEComponent
{
	class Mesh final : public Component
	{
	public:
		Mesh(std::wstring inputName,
			DEResource::VertexProcess::VertexData& inputVertexbuffer, std::shared_ptr<DEResource::IndexBuffer> inputIndexBuffer);
		Mesh(std::wstring inputName,
			DEResource::VertexProcess::VertexData& inputVertexbuffer, std::shared_ptr<DEResource::IndexBuffer> inputIndexBuffer,
			std::shared_ptr<DEResource::Topology> inputTopology);

		void SetMesh(DEResource::VertexProcess::VertexData& inputVertexbuffer,
			std::shared_ptr<DEResource::IndexBuffer> inputIndexBuffer) noexcept;
		void SetTopology(std::shared_ptr<DEResource::Topology> inputTopology) noexcept;
		[[nodiscard]] DEResource::VertexProcess::VertexData& GetVertexData() noexcept;
		[[nodiscard]] std::shared_ptr<DEResource::IndexBuffer> GetIndexBuffer() const noexcept;
		[[nodiscard]] std::shared_ptr<DEResource::Topology> GetTopology() const noexcept;
		[[nodiscard]] std::wstring GetName() const noexcept;

		void Bind() noxnd;

		void DrawComponentUI() override;
	private:
		std::wstring name;
		DEResource::VertexProcess::VertexData vertexData;
		std::shared_ptr<DEResource::IndexBuffer> indexBuffer;
		std::shared_ptr<DEResource::Topology> topology;

		std::shared_ptr<DEResource::ConstantBufferInRootSignature<UINT>> vertexDataIndexCB;
		std::shared_ptr<DEResource::StructuredBufferInHeap<char>> vertexBuffer;
	};
}
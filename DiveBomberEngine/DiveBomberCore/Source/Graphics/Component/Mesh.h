#pragma once
#include "..\GraphicsHeader.h"
#include "..\BindableObject\Vertex.h"

namespace DiveBomber::BindableObject
{
	class IndexBuffer;
	class Topology;
	template<typename C>
	class ConstantBuffer;
	template<typename C>
	class StructuredBufferInHeap;
}

namespace DiveBomber::BindableObject::VertexProcess
{
	class VertexData;
}

namespace DiveBomber::Component
{
	class Mesh final
	{
	public:
		Mesh(std::wstring inputName,
			BindableObject::VertexProcess::VertexData& inputVertexbuffer, std::shared_ptr<BindableObject::IndexBuffer> inputIndexBuffer);
		Mesh(std::wstring inputName,
			BindableObject::VertexProcess::VertexData& inputVertexbuffer, std::shared_ptr<BindableObject::IndexBuffer> inputIndexBuffer,
			std::shared_ptr<BindableObject::Topology> inputTopology);

		void SetMesh(BindableObject::VertexProcess::VertexData& inputVertexbuffer,
			std::shared_ptr<BindableObject::IndexBuffer> inputIndexBuffer) noexcept;
		void SetTopology(std::shared_ptr<BindableObject::Topology> inputTopology) noexcept;
		[[nodiscard]] BindableObject::VertexProcess::VertexData& GetVertexData() noexcept;
		[[nodiscard]] std::shared_ptr<BindableObject::IndexBuffer> GetIndexBuffer() const noexcept;
		[[nodiscard]] std::shared_ptr<BindableObject::Topology> GetTopology() const noexcept;
		[[nodiscard]] std::wstring GetName() const noexcept;

		void Bind() noxnd;
	private:
		std::wstring name;
		BindableObject::VertexProcess::VertexData vertexData;
		std::shared_ptr<BindableObject::IndexBuffer> indexBuffer;
		std::shared_ptr<BindableObject::Topology> topology;

		std::shared_ptr<BindableObject::ConstantBuffer<UINT>> vertexDataConstantBuffer;
		std::shared_ptr<BindableObject::StructuredBufferInHeap<char>> vertexBuffer;
	};
}
#pragma once
#include "..\GraphicsHeader.h"

namespace DiveBomber::BindableObject
{
	class VertexBuffer;
	class IndexBuffer;
	class Topology;
}

namespace DiveBomber::Component
{
	class Mesh final
	{
	public:
		Mesh(std::wstring inputName,
			std::shared_ptr<BindableObject::VertexBuffer> inputVertexbuffer, std::shared_ptr<BindableObject::IndexBuffer> inputIndexBuffer);
		Mesh(std::wstring inputName,
			std::shared_ptr<BindableObject::VertexBuffer> inputVertexbuffer, std::shared_ptr<BindableObject::IndexBuffer> inputIndexBuffer,
			std::shared_ptr<BindableObject::Topology> inputTopology);

		void SetMesh(std::shared_ptr<BindableObject::VertexBuffer> inputVertexbuffer,
			std::shared_ptr<BindableObject::IndexBuffer> inputIndexBuffer) noexcept;
		void SetTopology(std::shared_ptr<BindableObject::Topology> inputTopology) noexcept;
		[[nodiscard]] std::shared_ptr<BindableObject::VertexBuffer> GetVertexBuffer() const noexcept;
		[[nodiscard]] std::shared_ptr<BindableObject::IndexBuffer> GetIndexBuffer() const noexcept;
		[[nodiscard]] std::shared_ptr<BindableObject::Topology> GetTopology() const noexcept;
		[[nodiscard]] std::wstring GetName() const noexcept;

		void Bind() noxnd;
	private:
		std::wstring name;
		std::shared_ptr<BindableObject::VertexBuffer> vertexBuffer;
		std::shared_ptr<BindableObject::IndexBuffer> indexBuffer;
		std::shared_ptr<BindableObject::Topology> topology;
	};
}
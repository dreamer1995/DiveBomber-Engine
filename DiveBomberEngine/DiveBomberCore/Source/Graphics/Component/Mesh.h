#pragma once
#include "..\GraphicsHeader.h"

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

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
		Mesh(DEGraphics::Graphics & gfx, std::shared_ptr<BindableObject::VertexBuffer> inputVertexbuffer, std::shared_ptr<BindableObject::IndexBuffer> inputIndexBuffer);
		Mesh(DEGraphics::Graphics& gfx, std::shared_ptr<BindableObject::VertexBuffer> inputVertexbuffer, std::shared_ptr<BindableObject::IndexBuffer> inputIndexBuffer,
			std::shared_ptr<BindableObject::Topology> inputTopology);

		void SetMesh(std::shared_ptr<BindableObject::VertexBuffer> inputVertexbuffer,
			std::shared_ptr<BindableObject::IndexBuffer> inputIndexBuffer) noexcept;
		void SetTopology(std::shared_ptr<BindableObject::Topology> inputTopology) noexcept;
		[[nodiscard]] std::shared_ptr<BindableObject::VertexBuffer> GetVertexBuffer() const noexcept;
		[[nodiscard]] std::shared_ptr<BindableObject::IndexBuffer> GetIndexBuffer() const noexcept;
		[[nodiscard]] std::shared_ptr<BindableObject::Topology> GetTopology() const noexcept;

		void Bind(DEGraphics::Graphics& gfx) noxnd;
	private:
		std::shared_ptr<BindableObject::VertexBuffer> vertexBuffer;
		std::shared_ptr<BindableObject::IndexBuffer> indexBuffer;
		std::shared_ptr<BindableObject::Topology> topology;
	};
}
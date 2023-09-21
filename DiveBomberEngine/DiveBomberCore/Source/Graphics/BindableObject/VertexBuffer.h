#pragma once
#include "Bindable.h"
#include "Vertex.h"

namespace DiveBomber::BindableObject
{
	class VertexBuffer final : public Bindable
	{
	public:
		VertexBuffer(const std::string& inputTag, const VertexProcess::VertexData& vbuf);
		VertexBuffer(const VertexProcess::VertexData& vbuf);
		~VertexBuffer();

		void Bind() noxnd override;
		[[nodiscard]] const VertexProcess::VertexLayout& GetLayout() const noexcept;
		[[nodiscard]] static std::shared_ptr<VertexBuffer> Resolve(const std::string& tag,
			const VertexProcess::VertexData& vbuf);
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		[[nodiscard]] static std::string GenerateUID_(const std::string& tag);
		std::string tag;
		UINT stride;
		wrl::ComPtr<ID3D12Resource> vertexBuffer;
		VertexProcess::VertexLayout layout;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	};
}

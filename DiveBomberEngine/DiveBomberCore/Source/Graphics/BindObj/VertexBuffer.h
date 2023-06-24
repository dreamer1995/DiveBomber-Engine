#pragma once
#include "..\GraphicsResource.h"
#include "..\Graphics.h"
#include "Bindable.h"
#include "Vertex.h"

namespace DiveBomber::BindObj
{
	class VertexBuffer final : public Bindable
	{
	public:
		VertexBuffer(DEGraphics::Graphics& gfx, const std::string& inputTag, const VertexProcess::VertexData& vbuf);
		VertexBuffer(DEGraphics::Graphics& gfx, const VertexProcess::VertexData& vbuf);
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		[[nodiscard]] const VertexProcess::VertexLayout& GetLayout() const noexcept;
		[[nodiscard]] static std::shared_ptr<VertexBuffer> Resolve(DEGraphics::Graphics& gfx, const std::string& tag,
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
		wrl::ComPtr<ID3D12Resource> vertexUploadBuffer;
		VertexProcess::VertexLayout layout;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	};
}

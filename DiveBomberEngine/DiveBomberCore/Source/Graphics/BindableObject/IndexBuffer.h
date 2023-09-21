#pragma once
#include "Bindable.h"
#include "..\GraphicsHeader.h"

namespace DiveBomber::BindableObject
{
	class IndexBuffer final : public Bindable
	{
	public:
		IndexBuffer(const std::vector<unsigned short>& indices);
		IndexBuffer(std::string inputTag, const std::vector<unsigned short>& indices);
		~IndexBuffer();

		void Bind() noxnd override;
		[[nodiscard]] UINT GetCount() const noexcept;
		[[nodiscard]] static std::shared_ptr<IndexBuffer> Resolve(const std::string& tag,
			const std::vector<unsigned short>& indices);
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		[[nodiscard]] static std::string GenerateUID_(const std::string& tag);
		std::string tag;
		UINT count;
		wrl::ComPtr<ID3D12Resource> indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
	};
}
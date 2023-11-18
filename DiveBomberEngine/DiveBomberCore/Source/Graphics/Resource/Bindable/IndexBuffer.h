#pragma once
#include "..\Resource.h"
#include "Bindable.h"
#include "..\..\GraphicsHeader.h"

namespace DiveBomber::DEResource
{
	class IndexBuffer final : public Resource, public Bindable
	{
	public:
		IndexBuffer(const std::vector<unsigned short>& indices);
		IndexBuffer(std::wstring inputName, const std::vector<unsigned short>& indices);
		~IndexBuffer();

		void Bind() noxnd override;
		[[nodiscard]] UINT GetCount() const noexcept;
		template<typename...Ignore>
		[[nodiscard]] static std::string GenerateUID(const std::wstring& name, Ignore&&...ignore)
		{
			using namespace std::string_literals;
			return typeid(IndexBuffer).name() + "#"s + Utility::ToNarrow(name);
		}
		[[nodiscard]] std::string GetUID() const noexcept override;
	private:
		UINT count;
		wrl::ComPtr<ID3D12Resource> indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
	};
}
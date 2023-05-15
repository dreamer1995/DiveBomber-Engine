#pragma once
#include "Bindable.h"
#include "..\Graphics.h"

namespace DiveBomber::BindObj
{
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer(DEGraphics::Graphics& gfx, const std::vector<unsigned short>& indices);
		IndexBuffer(DEGraphics::Graphics& gfx, std::string tag, const std::vector<unsigned short>& indices);
		void Bind(DEGraphics::Graphics& gfx) noxnd override;
		UINT GetCount() const noexcept;
		static std::shared_ptr<IndexBuffer> Resolve(DEGraphics::Graphics& gfx, const std::string& tag,
			const std::vector<unsigned short>& indices);
		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		std::string GetUID() const noexcept override;
	private:
		static std::string GenerateUID_(const std::string& tag);
	protected:
		std::string tag;
		UINT count;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
	};
}
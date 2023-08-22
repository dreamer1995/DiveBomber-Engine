#pragma once
#include "..\GraphicsHeader.h"

#include <map>
namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::BindableObject
{
	class Texture;
}

namespace DiveBomber::Component
{
	class Material final
	{
	public:
		Material();
		void AddTexture(const std::shared_ptr<BindableObject::Texture> texture, UINT slot) noexcept;

		void Bind(DEGraphics::Graphics& gfx) noxnd;
	private:
		std::map<UINT, std::shared_ptr<BindableObject::Texture>> bindableTextureMap;
	};
}
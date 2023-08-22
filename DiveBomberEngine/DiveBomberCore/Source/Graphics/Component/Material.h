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
	class Material final
	{
	public:
		Material(DEGraphics::Graphics & gfx);

		void Bind(DEGraphics::Graphics& gfx) noxnd;
	private:
	};
}
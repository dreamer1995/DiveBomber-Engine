#pragma once
#include "..\Vertex.h"
#include "IndexedTriangleList.h"

namespace DiveBomber::BindObj
{
	class MiscShape final
	{
	public:
		[[nodiscard]] static IndexedTriangleList MakeDynamicArrow(VertexProcess::VertexLayout layout,const float length)
		{
			namespace dx = DirectX;
			float expansion = 0.2;
			VertexProcess::VertexData vd{ std::move(layout) };
			vd.EmplaceBack(dx::XMFLOAT3{ 0.0f,0.0f, 0.0f });
			vd.EmplaceBack(dx::XMFLOAT3{ 0.0f, 0.0f,length });
			vd.EmplaceBack(dx::XMFLOAT3{ 0.0f, 0.0f + expansion,length - expansion });
			vd.EmplaceBack(dx::XMFLOAT3{ 0.0f + expansion,0.0f,length - expansion });
			vd.EmplaceBack(dx::XMFLOAT3{ 0.0f, 0.0f - expansion,length - expansion });
			vd.EmplaceBack(dx::XMFLOAT3{ 0.0f - expansion,0.0f,length - expansion });

			return{
				std::move(vd),{
					0,1,
					1,2,   1,3,
					1,4,   1,5
				}
			};
		}

		[[nodiscard]] static IndexedTriangleList MakeArrow(const float length)
		{
			VertexProcess::VertexLayout vl;
			vl.Append(VertexProcess::VertexLayout::Position3D);

			return MakeDynamicArrow(std::move(vl), length);
		}
	};
}
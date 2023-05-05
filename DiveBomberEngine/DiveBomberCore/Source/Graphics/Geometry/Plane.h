#pragma once

#include <optional>
#include "Vertex.h"
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"
#include <array>

class Plane
{
public:
	enum class Type
	{
		SimpleQuad,
		PlaneTextured,
		PlaneTexturedNormal,
		PlaneTexturedTBN,
		TessellatedQuadTextured
	};
	static IndexedTriangleList MakeTesselatedTextured
	(Dvtx::VertexLayout layout, int divisions_x, int divisions_y, bool withTexture, bool withNormal, bool withTangent, bool tessellated)
	{
		namespace dx = DirectX;
		assert( divisions_x >= 1 );
		assert( divisions_y >= 1 );

		constexpr float width = 2.0f;
		constexpr float height = 2.0f;
		const int nVertices_x = divisions_x + 1;
		const int nVertices_y = divisions_y + 1;
		Dvtx::VertexBuffer vb{ std::move( layout ) };

		{
			const float side_x = width / 2.0f;
			const float side_y = height / 2.0f;
			const float divisionSize_x = width / float( divisions_x );
			const float divisionSize_y = height / float( divisions_y );
			float divisionSize_x_tc;
			float divisionSize_y_tc;
			if (withTexture)
			{
				divisionSize_x_tc = 1.0f / float(divisions_x);
				divisionSize_y_tc = 1.0f / float(divisions_y);
			}
			for( int y = 0,i = 0; y < nVertices_y; y++ )
			{
				const float y_pos = float( y ) * divisionSize_y - side_y;
				float y_pos_tc;
				if (withTexture)
					y_pos_tc = 1.0f - float(y) * divisionSize_y_tc;
				for( int x = 0; x < nVertices_x; x++,i++ )
				{
					const float x_pos = float( x ) * divisionSize_x - side_x;	
					if (withTexture)
					{
						float x_pos_tc = float(x) * divisionSize_x_tc;
						if (withNormal)
						{
							if (withTangent)
							{
								vb.EmplaceBack(
									dx::XMFLOAT3{ x_pos,y_pos,0.0f },
									dx::XMFLOAT3{ 1.0f,0.0f,0.0f },
									dx::XMFLOAT3{ 0.0f,-1.0f,0.0f },
									dx::XMFLOAT3{ 0.0f,0.0f,-1.0f },
									dx::XMFLOAT2{ x_pos_tc,y_pos_tc }
								);
							}
							else
							{
								vb.EmplaceBack(
									dx::XMFLOAT3{ x_pos,y_pos,0.0f },
									dx::XMFLOAT3{ 0.0f,0.0f,-1.0f },
									dx::XMFLOAT2{ x_pos_tc,y_pos_tc }
								);
							}
						}
						else
						{
							vb.EmplaceBack(
								dx::XMFLOAT3{ x_pos,y_pos,0.0f },
								dx::XMFLOAT2{ x_pos_tc,y_pos_tc }
							);
						}
					}
					else
					{
						vb.EmplaceBack(
							dx::XMFLOAT3{ x_pos,y_pos,0.0f }
						);
					}
				}
			}
		}

		std::vector<unsigned short> indices;
		indices.reserve( sq( divisions_x * divisions_y ) * 6 );
		{
			const auto vxy2i = [nVertices_x]( size_t x,size_t y )
			{
				return (unsigned short)(y * nVertices_x + x);
			};
			for( size_t y = 0; y < divisions_y; y++ )
			{
				for( size_t x = 0; x < divisions_x; x++ )
				{
					const std::array<unsigned short,4> indexArray =
					{ vxy2i( x,y ),vxy2i( x + 1,y ),vxy2i( x,y + 1 ),vxy2i( x + 1,y + 1 ) };
					indices.push_back( indexArray[0] );
					indices.push_back( indexArray[2] );
					if (tessellated)
					{
						indices.push_back(indexArray[3]);
						indices.push_back(indexArray[1]);
					}
					else
					{
						indices.push_back(indexArray[1]);
						indices.push_back(indexArray[1]);
						indices.push_back(indexArray[2]);
						indices.push_back(indexArray[3]);
					}
				}
			}
		}

		return{ std::move( vb ),std::move( indices ) };
	}
	static IndexedTriangleList Make(Type type = Type::SimpleQuad, UINT divisions = 1)
	{
		bool withTexture = false;
		bool withNormal = false;
		bool withTangent = false;
		bool tessellated = false;
		using Dvtx::VertexLayout;
		VertexLayout vl;
		vl.Append(VertexLayout::Position3D);
		switch (type)
		{
		case Type::TessellatedQuadTextured:
			tessellated = true;
			vl.Append(VertexLayout::Texture2D);
			withTexture = true;
			break;
		case Type::PlaneTexturedTBN:
		{
			vl.Append(VertexLayout::Tangent);
			vl.Append(VertexLayout::Binormal);
			withTangent = true;
		}	
		case Type::PlaneTexturedNormal:
		{
			vl.Append(VertexLayout::Normal);
			withNormal = true;
		}
		case Type::PlaneTextured:
		{
			vl.Append(VertexLayout::Texture2D);
			withTexture = true;
		}
		}

		return MakeTesselatedTextured(std::move(vl), divisions, divisions, withTexture, withNormal, withTangent, tessellated);
	}
};

#pragma once
#include "Vertex.h"
#include <vector>
#include <DirectXMath.h>

class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList( Dvtx::VertexBuffer verts_in,std::vector<unsigned short> indices_in )
		:
		vertices( std::move( verts_in ) ),
		indices( std::move( indices_in ) )
	{
		assert( vertices.Size() > 1 );
		assert(indices.size() % 3 == 0 || indices.size() % 2 == 0);
	}
	void Transform( DirectX::FXMMATRIX matrix )
	{
		using Elements = Dvtx::VertexLayout::ElementType;
		for( int i = 0; i < vertices.Size(); i++ )
		{
			auto& pos = vertices[i].Attr<Elements::Position3D>();
			DirectX::XMStoreFloat3(
				&pos,
				DirectX::XMVector3Transform( DirectX::XMLoadFloat3( &pos ),matrix )
			);
		}
	}
	void SetNormalsIndependentFlat() noxnd
	{
		using namespace DirectX;
		using Type = Dvtx::VertexLayout::ElementType;
		for( size_t i = 0; i < indices.size(); i += 3 )
		{
			auto v0 = vertices[indices[i]];
			auto v1 = vertices[indices[i + 1]];
			auto v2 = vertices[indices[i + 2]];
			const auto p0 = XMLoadFloat3( &v0.Attr<Type::Position3D>() );
			const auto p1 = XMLoadFloat3( &v1.Attr<Type::Position3D>() );
			const auto p2 = XMLoadFloat3( &v2.Attr<Type::Position3D>() );

			const auto n = XMVector3Normalize( XMVector3Cross( (p1 - p0),(p2 - p0) ) );
			
			XMStoreFloat3( &v0.Attr<Type::Normal>(),n );
			XMStoreFloat3( &v1.Attr<Type::Normal>(),n );
			XMStoreFloat3( &v2.Attr<Type::Normal>(),n );
		}
	}

	void SetTBNIndependentFlat() noxnd
	{
		using namespace DirectX;
		using Type = Dvtx::VertexLayout::ElementType;
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			auto v0 = vertices[indices[i]];
			auto v1 = vertices[indices[i + 1]];
			auto v2 = vertices[indices[i + 2]];
			const auto p0 = XMLoadFloat3(&v0.Attr<Type::Position3D>());
			const auto p1 = XMLoadFloat3(&v1.Attr<Type::Position3D>());
			const auto p2 = XMLoadFloat3(&v2.Attr<Type::Position3D>());

			const auto t0 = XMLoadFloat2(&v0.Attr<Type::Texture2D>());
			const auto t1 = XMLoadFloat2(&v1.Attr<Type::Texture2D>());
			const auto t2 = XMLoadFloat2(&v2.Attr<Type::Texture2D>());

			// Calculate the two vectors for this face.
			XMFLOAT3 p01, p02;
			XMStoreFloat3(&p01, (p1 - p0));
			XMStoreFloat3(&p02, (p2 - p0));

			// Calculate the tu and tv texture space vectors.
			XMFLOAT3 t01, t02;
			XMStoreFloat3(&t01, (t1 - t0));
			XMStoreFloat3(&t02, (t2 - t0));

			// Calculate the denominator of the tangent/binormal equation.
			const auto den = 1.0f / (t01.x * t02.y - t02.x * t01.y);

			// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
			XMFLOAT3 tangent, binormal;

			tangent.x = (t02.y * p01.x - t01.y * p02.x) * den;
			tangent.y = (t02.y * p01.y - t01.y * p02.y) * den;
			tangent.z = (t02.y * p01.z - t01.y * p02.z) * den;

			binormal.x = (t01.x * p02.x - t02.x * p01.x) * den;
			binormal.y = (t01.x * p02.y - t02.x * p01.y) * den;
			binormal.z = (t01.x * p02.z - t02.x * p01.z) * den;

			const auto t = XMVector3Normalize(XMLoadFloat3(&tangent));
			XMStoreFloat3(&v0.Attr<Type::Tangent>(), t);
			XMStoreFloat3(&v1.Attr<Type::Tangent>(), t);
			XMStoreFloat3(&v2.Attr<Type::Tangent>(), t);

			const auto b = XMVector3Normalize(XMLoadFloat3(&binormal));
			XMStoreFloat3(&v0.Attr<Type::Binormal>(), b);
			XMStoreFloat3(&v1.Attr<Type::Binormal>(), b);
			XMStoreFloat3(&v2.Attr<Type::Binormal>(), b);

			const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));
			XMStoreFloat3(&v0.Attr<Type::Normal>(), n);
			XMStoreFloat3(&v1.Attr<Type::Normal>(), n);
			XMStoreFloat3(&v2.Attr<Type::Normal>(), n);
		}
	}

public:
	Dvtx::VertexBuffer vertices;
	std::vector<unsigned short> indices;
};
#pragma once
#include "..\Vertex.h"
#include "IndexedTriangleList.h"
#include "..\.\..\..\Utility\DEMath.h"

#include <optional>
#include <DirectXMath.h>

namespace DiveBomber::BindObj
{
	class Sphere
	{
	public:
		static IndexedTriangleList MakeTesselated(VertexProcess::VertexLayout layout, int latDiv, int longDiv, bool withNormal)
		{
			namespace dx = DirectX;
			assert(latDiv >= 3);
			assert(longDiv >= 3);

			constexpr float radius = 1.0f;
			const auto base = dx::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
			const float lattitudeAngle = Utility::PI / latDiv;
			const float longitudeAngle = 2.0f * Utility::PI / longDiv;

			VertexProcess::VertexData vd{ std::move(layout) };
			for (int iLat = 1; iLat < latDiv; iLat++)
			{
				const auto latBase = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationX(lattitudeAngle * iLat)
				);
				for (int iLong = 0; iLong < longDiv; iLong++)
				{
					dx::XMFLOAT3 calculatedPos;
					auto v = dx::XMVector3Transform(
						latBase,
						dx::XMMatrixRotationZ(longitudeAngle * iLong)
					);
					dx::XMStoreFloat3(&calculatedPos, v);
					if (withNormal)
					{
						dx::XMFLOAT3 calculatedNor;
						dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(v));
						vd.EmplaceBack(calculatedPos, calculatedNor);
					}
					else
					{
						vd.EmplaceBack(calculatedPos);
					}
				}
			}

			// add the cap vertices
			const auto iNorthPole = (unsigned short)vd.Size();
			{
				dx::XMFLOAT3 northPos;
				dx::XMStoreFloat3(&northPos, base);
				if (withNormal)
				{
					dx::XMFLOAT3 calculatedNor;
					dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(base));
					vd.EmplaceBack(northPos, calculatedNor);
				}
				else
				{
					vd.EmplaceBack(northPos);
				}
			}
			const auto iSouthPole = (unsigned short)vd.Size();
			{
				dx::XMFLOAT3 southPos;
				dx::XMStoreFloat3(&southPos, dx::XMVectorNegate(base));
				if (withNormal)
				{
					dx::XMFLOAT3 calculatedNor;
					dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(dx::XMVectorNegate(base)));
					vd.EmplaceBack(southPos, calculatedNor);
				}
				else
				{
					vd.EmplaceBack(southPos);
				}
			}

			const auto calcIdx = [latDiv, longDiv](unsigned short iLat, unsigned short iLong)
			{ return iLat * longDiv + iLong; };
			std::vector<unsigned short> indices;
			for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
			{
				for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
				{
					indices.push_back(calcIdx(iLat, iLong));
					indices.push_back(calcIdx(iLat + 1, iLong));
					indices.push_back(calcIdx(iLat, iLong + 1));
					indices.push_back(calcIdx(iLat, iLong + 1));
					indices.push_back(calcIdx(iLat + 1, iLong));
					indices.push_back(calcIdx(iLat + 1, iLong + 1));
				}
				// wrap band
				indices.push_back(calcIdx(iLat, longDiv - 1));
				indices.push_back(calcIdx(iLat + 1, longDiv - 1));
				indices.push_back(calcIdx(iLat, 0));
				indices.push_back(calcIdx(iLat, 0));
				indices.push_back(calcIdx(iLat + 1, longDiv - 1));
				indices.push_back(calcIdx(iLat + 1, 0));
			}

			// cap fans
			for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
			{
				// north
				indices.push_back(iNorthPole);
				indices.push_back(calcIdx(0, iLong));
				indices.push_back(calcIdx(0, iLong + 1));
				// south
				indices.push_back(calcIdx(latDiv - 2, iLong + 1));
				indices.push_back(calcIdx(latDiv - 2, iLong));
				indices.push_back(iSouthPole);
			}
			// wrap triangles
			// north
			indices.push_back(iNorthPole);
			indices.push_back(calcIdx(0, longDiv - 1));
			indices.push_back(calcIdx(0, 0));
			// south
			indices.push_back(calcIdx(latDiv - 2, 0));
			indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
			indices.push_back(iSouthPole);

			return { std::move(vd),std::move(indices) };
		}

		static IndexedTriangleList MakeTesselatedNormalUV(VertexProcess::VertexLayout layout, int latDiv, int longDiv, bool withTangent)
		{
			namespace dx = DirectX;
			assert(latDiv >= 3);
			assert(longDiv >= 3);

			constexpr float radius = 1.0f;
			const auto base = dx::XMVectorSet(0.0f, radius, 0.0f, 0.0f);
			const float lattitudeAngle = Utility::PI / latDiv;
			const float intervalV = 1.0f / latDiv;
			const float longitudeAngle = 2.0f * Utility::PI / longDiv;
			const float intervalU = 1.0f / longDiv;

			VertexProcess::VertexData vd{ std::move(layout) };
			for (int iLat = 1; iLat < latDiv; iLat++)
			{
				const auto latBase = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationX(lattitudeAngle * iLat)
				);
				const float vSum = intervalV * iLat;
				DirectX::XMFLOAT3 _calculatedPos, _calculatedNor;
				DirectX::XMFLOAT3 _calculatedTan, _calculatedBin;
				for (int iLong = 0; iLong < longDiv; iLong++)
				{
					auto v = dx::XMVector3Transform(
						latBase,
						dx::XMMatrixRotationY(longitudeAngle * iLong)
					);
					if (iLong != 0)
					{
						dx::XMFLOAT3 calculatedPos;
						dx::XMStoreFloat3(&calculatedPos, v);
						dx::XMFLOAT3 calculatedNor;
						dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(v));
						dx::XMFLOAT2 calculatedUV = { intervalU * iLong,vSum };
						if (withTangent)
						{
							dx::XMFLOAT3 calculatedTan;
							dx::XMStoreFloat3(&calculatedTan, dx::XMVector3Normalize({ calculatedPos.z,0.0f,-calculatedPos.x }));
							dx::XMFLOAT3 calculatedBin;
							if (calculatedPos.y > 0)
							{
								dx::XMStoreFloat3(&calculatedBin, dx::XMVector3Normalize({ calculatedPos.x,
								-(calculatedPos.x * calculatedPos.x + calculatedPos.z * calculatedPos.z) / calculatedPos.y,
								calculatedPos.z }));
							}
							else if (calculatedPos.y < 0)
							{
								dx::XMStoreFloat3(&calculatedBin, dx::XMVector3Normalize({ -calculatedPos.x,
								calculatedPos.x * calculatedPos.x + calculatedPos.z * calculatedPos.z / calculatedPos.y,
								-calculatedPos.z }));
							}
							else
							{
								dx::XMStoreFloat3(&calculatedBin, { 0.0f,1.0f,0.0f });
							}
							vd.EmplaceBack(calculatedPos, calculatedNor, calculatedTan, calculatedBin, calculatedUV);
						}
						else
						{
							vd.EmplaceBack(calculatedPos, calculatedNor, calculatedUV);
						}
					}
					else
					{
						dx::XMStoreFloat3(&_calculatedPos, v);
						dx::XMStoreFloat3(&_calculatedNor, dx::XMVector3Normalize(v));
						dx::XMFLOAT2 calculatedUV = { 0.0f,vSum };
						if (withTangent)
						{
							dx::XMStoreFloat3(&_calculatedTan, dx::XMVector3Normalize({ _calculatedPos.z,0.0f,-_calculatedPos.x }));
							if (_calculatedPos.y > 0)
							{
								dx::XMStoreFloat3(&_calculatedBin, dx::XMVector3Normalize({ _calculatedPos.x,
								-(_calculatedPos.x * _calculatedPos.x + _calculatedPos.z * _calculatedPos.z) / _calculatedPos.y,
								_calculatedPos.z }));
							}
							else if (_calculatedPos.y < 0)
							{
								dx::XMStoreFloat3(&_calculatedBin, dx::XMVector3Normalize({ -_calculatedPos.x,
								_calculatedPos.x * _calculatedPos.x + _calculatedPos.z * _calculatedPos.z / _calculatedPos.y,
								-_calculatedPos.z }));
							}
							else
							{
								dx::XMStoreFloat3(&_calculatedBin, { 0.0f,1.0f,0.0f });
							}
							vd.EmplaceBack(_calculatedPos, _calculatedNor, _calculatedTan, _calculatedBin, calculatedUV);
						}
						else
						{
							vd.EmplaceBack(_calculatedPos, _calculatedNor, calculatedUV);
						}
					}
				}
				dx::XMFLOAT2 calculatedUV = { 1.0f,vSum };
				if (withTangent)
				{
					vd.EmplaceBack(_calculatedPos, _calculatedNor, _calculatedTan, _calculatedBin, calculatedUV);
				}
				else
				{
					vd.EmplaceBack(_calculatedPos, _calculatedNor, calculatedUV);
				}
			}

			// add the cap vertices
			const auto iNorthPole = (unsigned short)vd.Size();
			{
				dx::XMFLOAT3 northPos;
				dx::XMStoreFloat3(&northPos, base);
				dx::XMFLOAT3 calculatedNor;
				dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(base));
				for (int iLong = 0; iLong < longDiv; iLong++)
				{
					dx::XMFLOAT2 calculatedUV = { intervalU * iLong,0.0f };
					if (withTangent)
					{
						const auto latBase = dx::XMVector3Transform(
							base,
							dx::XMMatrixRotationX(lattitudeAngle)
						);
						auto v = dx::XMVector3Transform(
							latBase,
							dx::XMMatrixRotationY(longitudeAngle * iLong)
						);
						dx::XMFLOAT3 _calculatedPos;
						dx::XMStoreFloat3(&_calculatedPos, v);
						dx::XMFLOAT3 calculatedTan;
						dx::XMStoreFloat3(&calculatedTan, dx::XMVector3Normalize({ _calculatedPos.z,0.0,-_calculatedPos.x }));
						dx::XMFLOAT3 calculatedBin;
						dx::XMStoreFloat3(&calculatedBin, dx::XMVector3Normalize({ _calculatedPos.x,0.0,_calculatedPos.z }));
						vd.EmplaceBack(northPos, calculatedNor, calculatedTan, calculatedBin, calculatedUV);
					}
					else
					{
						vd.EmplaceBack(northPos, calculatedNor, calculatedUV);
					}
				}
			}
			const auto iSouthPole = (unsigned short)vd.Size();
			{
				dx::XMFLOAT3 southPos;
				dx::XMStoreFloat3(&southPos, dx::XMVectorNegate(base));
				dx::XMFLOAT3 calculatedNor;
				dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(dx::XMVectorNegate(base)));
				for (int iLong = 0; iLong < longDiv; iLong++)
				{
					dx::XMFLOAT2 calculatedUV = { intervalU * iLong,1.0f };
					if (withTangent)
					{
						const auto latBase = dx::XMVector3Transform(
							base,
							dx::XMMatrixRotationX(lattitudeAngle)
						);
						auto v = dx::XMVector3Transform(
							latBase,
							dx::XMMatrixRotationY(longitudeAngle * iLong)
						);
						dx::XMFLOAT3 _calculatedPos;
						dx::XMStoreFloat3(&_calculatedPos, v);
						dx::XMFLOAT3 calculatedTan;
						dx::XMStoreFloat3(&calculatedTan, dx::XMVector3Normalize({ _calculatedPos.z,0.0,-_calculatedPos.x }));
						dx::XMFLOAT3 calculatedBin;
						dx::XMStoreFloat3(&calculatedBin, dx::XMVector3Normalize({ -_calculatedPos.x,0.0,-_calculatedPos.z }));
						vd.EmplaceBack(southPos, calculatedNor, calculatedTan, calculatedBin, calculatedUV);
					}
					else
					{
						vd.EmplaceBack(southPos, calculatedNor, calculatedUV);
					}
				}
			}

			const auto calcIdx = [latDiv, longDiv](unsigned short iLat, unsigned short iLong)
			{ return iLat * (longDiv + 1) + iLong; };
			std::vector<unsigned short> indices;
			for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
			{
				for (unsigned short iLong = 0; iLong < longDiv; iLong++)
				{
					indices.push_back(calcIdx(iLat, iLong));
					indices.push_back(calcIdx(iLat + 1, iLong));
					indices.push_back(calcIdx(iLat, iLong + 1));
					indices.push_back(calcIdx(iLat, iLong + 1));
					indices.push_back(calcIdx(iLat + 1, iLong));
					indices.push_back(calcIdx(iLat + 1, iLong + 1));
				}
			}

			// cap fans
			for (unsigned short iLong = 0; iLong < longDiv; iLong++)
			{
				// north
				indices.push_back(iNorthPole + iLong);
				indices.push_back(calcIdx(0, iLong));
				indices.push_back(calcIdx(0, iLong + 1));
				// south
				indices.push_back(calcIdx(latDiv - 2, iLong + 1));
				indices.push_back(calcIdx(latDiv - 2, iLong));
				indices.push_back(iSouthPole + iLong);
			}

			return { std::move(vd),std::move(indices) };
		}

		static IndexedTriangleList Make(std::optional<VertexProcess::VertexLayout> layout = std::nullopt, bool withTangent = false)
		{
			using Element = VertexProcess::VertexLayout::ElementType;
			if (!layout)
			{
				layout = VertexProcess::VertexLayout{}.Append(Element::Position3D);
			}
			return MakeTesselated(std::move(*layout), 12, 24, withTangent);
		}

		static IndexedTriangleList MakeNormalUVed(std::optional<VertexProcess::VertexLayout> layout = std::nullopt, bool withTangent = false)
		{
			if (!layout)
			{
				layout = VertexProcess::VertexLayout{}
					.Append(VertexProcess::VertexLayout::Position3D)
					.Append(VertexProcess::VertexLayout::Normal)
					.Append(VertexProcess::VertexLayout::Texture2D);
			}

			return MakeTesselatedNormalUV(std::move(*layout), 48, 96, withTangent);
		}
	};
}
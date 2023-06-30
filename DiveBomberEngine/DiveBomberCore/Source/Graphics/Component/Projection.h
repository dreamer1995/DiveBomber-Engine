#pragma once
#include "..\Graphics.h"
//#include "Frustum.h"

#include <DirectXMath.h>

namespace DiveBomber::Component
{
	class Projection final
	{
	public:
		struct ProjectionAttributes
		{
			float aspectRatio = 16.0f / 9.0f;
			float nearPlane = 0.5f;
			float farPlane = 400.0f;
			bool isPerspective = true;
			float FOV = 60.0f / 180.0f * Utility::PI;
		};

	public:
		Projection(DEGraphics::Graphics& gfx, ProjectionAttributes projectionAttributes);
		DirectX::XMMATRIX GetMatrix(DEGraphics::Graphics& gfx) const;
		//void RenderWidgets(DEGraphics::Graphics& gfx);
		void SetPos(DirectX::XMFLOAT3);
		void SetRotation(DirectX::XMFLOAT3);
		void Submit(size_t channel) const;
		//void LinkTechniques(Rgph::RenderGraph& rg);
		void Reset(DEGraphics::Graphics& gfx);
		void SetProjection(float inputFOV, float inputAspectRatio, float inputNearPlane, float inputFarPlane);
		DirectX::XMFLOAT2 GetFarNearPlane() const;
		float GetFOV() const;
		float GetAspectRatio() const;
		void SetOffsetPixels(float offsetX, float offsetY) noxnd;
	private:
		float width;
		float height;
		float nearPlane;
		float farPlane;
		float aspectRatio;
		bool isPerspective;
		float FOV;

		float homeWidth;
		float homeHeight;
		float homeNearPlane;
		float homeFarPlane;
		float homeAspectRatio;
		float homeFOV;

		//std::unique_ptr<Frustum> frust;

		float offsetPixelX = 0.0f;
		float offsetPixelY = 0.0f;
	};
}
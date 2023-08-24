#pragma once
#include "..\..\Window\DEWindows.h"
#include "..\..\Utility\Common.h"
#include "..\..\Utility\DEMath.h"
#include "..\..\..\Config\SystemConfig.h"
//#include "Frustum.h"

#include <memory>
#include <DirectXMath.h>

namespace DiveBomber::DEGraphics
{
	class Graphics;
}

namespace DiveBomber::Component
{
	class Projection final
	{
	public:
		struct ProjectionAttributes
		{
			float aspectRatio = (float)MainWindowWidth / MainWindowHeight;
			float nearPlane = 0.01f;
			float farPlane = 400.0f;
			bool isPerspective = true;
			float FOV = 60.0f / 180.0f * Utility::PI;
		};

	public:
		Projection(DEGraphics::Graphics& gfx, ProjectionAttributes projectionAttributes);
		[[nodiscard]] DirectX::XMMATRIX GetMatrix(DEGraphics::Graphics& gfx) const;
		[[nodiscard]] DirectX::XMMATRIX GetMatrix(UINT renderTargetWidth, UINT renderTargetHeight) const;
		//void RenderWidgets(DEGraphics::Graphics& gfx);
		void SetPos(const DirectX::XMFLOAT3);
		void SetRotation(const DirectX::XMFLOAT3);
		void Submit(size_t channel) const;
		//void LinkTechniques(Rgph::RenderGraph& rg);
		void Reset(const DEGraphics::Graphics& gfx);
		void SetProjection(const float inputFOV, const float inputAspectRatio,
			const float inputNearPlane, const float inputFarPlane);
		[[nodiscard]] DirectX::XMFLOAT2 GetFarNearPlane() const;
		float GetFOV() const;
		float GetAspectRatio() const;
		void SetOffsetPixels(const float offsetX, const float offsetY) noexcept;
		void ResizeAspectRatio(const DEGraphics::Graphics& gfx) noexcept;
		void ResizeAspectRatio(const UINT inputWidth, const UINT inputHeight) noexcept;
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
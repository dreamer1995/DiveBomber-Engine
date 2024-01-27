#pragma once
#include "..\..\Window\DEWindows.h"
#include "..\..\Utility\Common.h"
#include "..\..\Utility\DEMath.h"
#include "..\..\..\Config\SystemConfig.h"
//#include "Frustum.h"

#include <memory>
#include <DirectXMath.h>

namespace DiveBomber::DEComponent
{
	class Projection final
	{
	public:
		struct PerspectiveAttributes
		{
			float FOV = 60.0f / 180.0f * Utility::PI;
			float aspectRatio = (float)MainWindowWidth / MainWindowHeight;
		};

		struct OrthographicAttributes
		{
			float width = MainWindowWidth;
			float height = MainWindowHeight;
		};

		struct ProjectionAttributes
		{
			float nearPlane = 0.01f;
			float farPlane = 400.0f;
			bool isPerspective = true;
			union
			{
				PerspectiveAttributes perspectiveAttributes;
				OrthographicAttributes orthographicAttributes;
			};
		};

	public:
		Projection(ProjectionAttributes inputAttributes);
		[[nodiscard]] DirectX::XMMATRIX GetMatrix() const;
		[[nodiscard]] DirectX::XMMATRIX GetMatrix(UINT renderTargetWidth, UINT renderTargetHeight) const;
		void SetPos(const DirectX::XMFLOAT3);
		void SetRotation(const DirectX::XMFLOAT3);
		void Submit(size_t channel) const;
		//void LinkTechniques(Rgph::RenderGraph& rg);
		void Reset();
		void SetPerspectiveProjection(const float FOV, const float aspectRatio,
			const float nearPlane, const float farPlane);
		void SetOrthographicProjection(const float width, const float height,
			const float nearPlane, const float farPlane);
		[[nodiscard]] DirectX::XMFLOAT2 GetFarNearPlane() const;
		float GetFOV() const;
		float GetAspectRatio() const;
		void SetOffsetPixels(const float offsetX, const float offsetY) noexcept;
		void ResizeAspectRatio() noexcept;
		void ResizeAspectRatio(const UINT inputWidth, const UINT inputHeight) noexcept;

		void DrawComponentUI();

	private:
		ProjectionAttributes homeAttributes;
		ProjectionAttributes attributes;

		//std::unique_ptr<Frustum> frust;

		float offsetPixelX = 0.0f;
		float offsetPixelY = 0.0f;
	};
}
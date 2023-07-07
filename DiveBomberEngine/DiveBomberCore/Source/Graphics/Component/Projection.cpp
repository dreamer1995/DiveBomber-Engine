#include "Projection.h"
//#include "imgui/imgui.h"

namespace DiveBomber::Component
{
	using namespace DEGraphics;

	Projection::Projection(Graphics& gfx, ProjectionAttributes attributes)
	{
		aspectRatio = attributes.aspectRatio;
		nearPlane = attributes.nearPlane;
		farPlane = attributes.farPlane;
		isPerspective = attributes.isPerspective;
		FOV = attributes.FOV;

		homeNearPlane = nearPlane;
		homeFarPlane = farPlane;
		homeAspectRatio = aspectRatio;
		homeFOV = FOV;

		if (isPerspective)
		{
			height = 2 * nearPlane * std::tan(FOV / 2.0f);
			width = height * aspectRatio;
			//frust = std::make_unique<Frustum>(gfx, width, height, nearZ, farZ, isPerspective);
			homeWidth = width;
			homeHeight = height;
		}
		else
		{
			// ?
			width = aspectRatio;
			height = FOV;
			homeWidth = width;
			homeHeight = height;
			//frust = std::make_unique<Frustum>(gfx, width, height, nearZ, farZ, isPerspective);
		}
	}

	// need to be re-designed, make sure the cost is fine
	DirectX::XMMATRIX Projection::GetMatrix(Graphics& gfx) const
	{
		return GetMatrix(gfx.GetWidth(), gfx.GetHeight());
	}

	DirectX::XMMATRIX Projection::GetMatrix(UINT renderTargetWidth, UINT renderTargetHeight) const
	{
		if (isPerspective)
		{
			DirectX::XMFLOAT4X4 projArray4x4;
			DirectX::XMStoreFloat4x4(&projArray4x4, DirectX::XMMatrixPerspectiveFovLH(FOV, aspectRatio, nearPlane, farPlane));
			//float vDirection = -1 //For OpenGL? Fuck mesxiah;
			if (EnableTAA)
			{
				projArray4x4._31 = -2.0f * offsetPixelX / renderTargetWidth;
				projArray4x4._32 = 2.0f * offsetPixelY / renderTargetHeight;
			}
			return DirectX::XMLoadFloat4x4(&projArray4x4);
		}
		else
		{
			DirectX::XMFLOAT4X4 projArray4x4;
			DirectX::XMStoreFloat4x4(&projArray4x4, DirectX::XMMatrixOrthographicLH(width, height, nearPlane, farPlane));

			if (EnableTAA)
			{
				projArray4x4._31 = -2.0f * offsetPixelX / renderTargetWidth;
				projArray4x4._32 = -2.0f * offsetPixelY / renderTargetHeight;
			}
			return DirectX::XMLoadFloat4x4(&projArray4x4);
		}
	}

	//void Projection::RenderWidgets(Graphics& gfx)
	//{
	//	bool dirty = false;
	//	const auto dcheck = [&dirty](bool d) { dirty = dirty || d; };

	//	if (isPerspective)
	//	{
	//		ImGui::Text("Projection");
	//		dcheck(ImGui::SliderFloat("FOV", &UIFOV, 1.0f, 179.0f, "%.0f"));
	//		dcheck(ImGui::SliderFloat("Aspect", &aspect, 0.01f, 1000.0f, "%.7f", 10.0f));
	//		dcheck(ImGui::SliderFloat("Near Z", &nearZ, 0.01f, farZ - 0.01f, "%.2f", 4.0f));
	//		dcheck(ImGui::SliderFloat("Far Z", &farZ, nearZ + 0.01f, 400.0f, "%.2f", 4.0f));
	//	}
	//	else
	//	{
	//		ImGui::Text("Projection");
	//		dcheck(ImGui::SliderFloat("Width", &width, 0.01f, 4096.f, "%.2f", 1.0f));
	//		dcheck(ImGui::SliderFloat("Height", &height, 0.01f, 4096.0f, "%.2f", 1.0f));
	//		dcheck(ImGui::SliderFloat("Near Z", &nearZ, 0.01f, farZ - 0.01f, "%.2f", 1.0f));
	//		dcheck(ImGui::SliderFloat("Far Z", &farZ, nearZ + 0.01f, 400.0f, "%.2f", 1.0f));
	//	}

	//	if (dirty)
	//	{
	//		if (isPerspective)
	//		{
	//			FOV = UIFOV / 180.0f * PI;
	//			height = 2 * nearZ * std::tan(FOV / 2.0f);
	//			width = height * aspect;
	//		}
	//		frust->SetVertices(gfx, width, height, nearZ, farZ);
	//	}
	//}

	void Projection::SetPos(const DirectX::XMFLOAT3 pos)
	{
		//frust->SetPos(pos);
	}

	void Projection::SetRotation(const DirectX::XMFLOAT3 rot)
	{
		//frust->SetRotation(rot);
	}

	void Projection::Submit(size_t channel) const
	{
		//frust->Submit(channel);
	}

	//void Projection::LinkTechniques(Rgph::RenderGraph& rg)
	//{
	//	frust->LinkTechniques(rg);
	//}

	void Projection::Reset(const Graphics& gfx)
	{
		width = homeWidth;
		height = homeHeight;
		nearPlane = homeNearPlane;
		farPlane = homeFarPlane;
		aspectRatio = homeAspectRatio;
		FOV = homeFOV;

		SetProjection(FOV, aspectRatio, nearPlane, farPlane);
		//frust->SetVertices(gfx, width, height, nearZ, farZ);
	}

	void Projection::SetProjection(const float inputFOV, const float inputAspectRatio,
		const float inputNearPlane, const float inputFarPlane)
	{
		FOV = inputFOV;
		aspectRatio = inputAspectRatio;
		nearPlane = inputNearPlane;
		farPlane = inputFarPlane;
		if (isPerspective)
		{
			height = 2 * nearPlane * std::tan(FOV / 2.0f);
			width = height * aspectRatio;
		}
		else
		{
			// ?
			width = FOV;
			height = aspectRatio;
		}
	}

	DirectX::XMFLOAT2 Projection::GetFarNearPlane() const
	{
		// be awared!!! swapped!!!
		return { nearPlane,farPlane };
	}

	float Projection::GetFOV() const
	{
		return FOV;
	}

	float Projection::GetAspectRatio() const
	{
		return aspectRatio;
	}

	void Projection::SetOffsetPixels(const float offsetX, const float offsetY) noexcept
	{
		offsetPixelX = offsetX;
		offsetPixelY = offsetY;
	}

	void Projection::ResizeAspectRatio(const Graphics& gfx) noexcept
	{
		ResizeAspectRatio(gfx.GetWidth(), gfx.GetHeight());
	}

	void Projection::ResizeAspectRatio(const UINT inputWidth, const UINT inputHeight) noexcept
	{
		float inputAspectRatio = (float)inputWidth / inputHeight;
		SetProjection(FOV, inputAspectRatio, nearPlane, farPlane);
	}
}
#include "Projection.h"

#include <algorithm>
#include <..\imgui\imgui.h>

namespace DiveBomber::DEComponent
{
	using namespace DEGraphics;

	Projection::Projection(ProjectionAttributes inputAttributes)
		:
		homeAttributes(inputAttributes),
		attributes(homeAttributes)
	{
		// frust = std...
	}

	// need to be re-designed, make sure the cost is fine
	DirectX::XMMATRIX Projection::GetMatrix() const
	{
		return GetMatrix(Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight());
	}

	DirectX::XMMATRIX Projection::GetMatrix(UINT renderTargetWidth, UINT renderTargetHeight) const
	{
		if (attributes.isPerspective)
		{	
			DirectX::XMFLOAT4X4 projArray4x4;
			DirectX::XMStoreFloat4x4(&projArray4x4, DirectX::XMMatrixPerspectiveFovLH(
				attributes.perspectiveAttributes.FOV, attributes.perspectiveAttributes.aspectRatio,
				attributes.nearPlane, attributes.farPlane));
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
			DirectX::XMStoreFloat4x4(&projArray4x4, DirectX::XMMatrixOrthographicLH(
				attributes.orthographicAttributes.width, attributes.orthographicAttributes.height,
				attributes.nearPlane, attributes.farPlane));

			if (EnableTAA)
			{
				projArray4x4._31 = -2.0f * offsetPixelX / renderTargetWidth;
				projArray4x4._32 = -2.0f * offsetPixelY / renderTargetHeight;
			}
			return DirectX::XMLoadFloat4x4(&projArray4x4);
		}
	}

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

	void Projection::Reset()
	{
		attributes = homeAttributes;

		//frust->SetVertices(gfx, width, height, nearZ, farZ);
	}

	void Projection::SetPerspectiveProjection(const float FOV, const float aspectRatio,
		const float nearPlane, const float farPlane)
	{
		attributes.perspectiveAttributes.FOV = FOV;
		attributes.perspectiveAttributes.aspectRatio = aspectRatio;
		attributes.nearPlane = nearPlane;
		attributes.farPlane = farPlane;
	}

	void Projection::SetOrthographicProjection(const float width, const float height,
		const float nearPlane, const float farPlane)
	{
		attributes.orthographicAttributes.width = width;
		attributes.orthographicAttributes.height = height;
		attributes.nearPlane = nearPlane;
		attributes.farPlane = farPlane;
	}

	DirectX::XMFLOAT2 Projection::GetFarNearPlane() const
	{
		// be awared!!! swapped!!!
		return { attributes.nearPlane,attributes.farPlane };
	}

	float Projection::GetFOV() const
	{
		return attributes.perspectiveAttributes.FOV;
	}

	float Projection::GetAspectRatio() const
	{
		return attributes.perspectiveAttributes.aspectRatio;
	}

	void Projection::SetOffsetPixels(const float offsetX, const float offsetY) noexcept
	{
		offsetPixelX = offsetX;
		offsetPixelY = offsetY;
	}

	void Projection::ResizeAspectRatio() noexcept
	{
		ResizeAspectRatio(Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight());
	}

	void Projection::ResizeAspectRatio(const UINT inputWidth, const UINT inputHeight) noexcept
	{
		float inputAspectRatio = std::max(0.00001f, (float)inputWidth) / std::max(0.00001f, (float)inputHeight);
		attributes.perspectiveAttributes.aspectRatio = inputAspectRatio;
	}

	void Projection::DrawComponentUI()
	{
		float FOVInDegree = 0.0f;
		bool dirty = false;
		const auto dcheck = [&dirty](bool d) { dirty = dirty || d; };

		if (attributes.isPerspective)
		{
			FOVInDegree = attributes.perspectiveAttributes.FOV * 180.0f / Utility::PI;
			ImGui::Text("Projection");
			dcheck(ImGui::SliderFloat("FOV", &FOVInDegree, 1.0f, 179.0f, "%.0f"));
			dcheck(ImGui::SliderFloat("Near Z", &attributes.nearPlane, 0.01f, attributes.farPlane - 0.01f, "%.2f", ImGuiSliderFlags_Logarithmic));
			dcheck(ImGui::SliderFloat("Far Z", &attributes.farPlane, attributes.nearPlane + 0.01f, 400.0f, "%.2f", ImGuiSliderFlags_Logarithmic));
		}
		else
		{
			ImGui::Text("Projection");
			dcheck(ImGui::SliderFloat("Width", &attributes.orthographicAttributes.width, 0.01f, 4096.f, "%.2f"));
			dcheck(ImGui::SliderFloat("Height", &attributes.orthographicAttributes.height, 0.01f, 4096.0f, "%.2f"));
			dcheck(ImGui::SliderFloat("Near Z", &attributes.nearPlane, 0.01f, attributes.farPlane - 0.01f, "%.2f"));
			dcheck(ImGui::SliderFloat("Far Z", &attributes.farPlane, attributes.nearPlane + 0.01f, 400.0f, "%.2f"));
		}

		if (dirty)
		{
			if (attributes.isPerspective)
			{
				attributes.perspectiveAttributes.FOV = FOVInDegree / 180.0f * Utility::PI;
			}
			//frust->SetVertices(gfx, width, height, nearZ, farZ);
		}
	}
}
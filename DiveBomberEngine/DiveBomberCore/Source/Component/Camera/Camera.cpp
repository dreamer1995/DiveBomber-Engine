#include "Camera.h"

#include "..\..\Graphics\Graphics.h"
#include "..\..\Graphics\GraphicResource\Bindable\ConstantBufferInRootSignature.h"
#include <..\imgui\imgui.h>

namespace DiveBomber::DEComponent
{
	using namespace DEGraphics;
	using namespace Utility;
	using namespace GraphicResource;
	namespace dx = DirectX;

	Camera::Camera(std::wstring inputName, CameraAttributes inputAttributes) noexcept
		:
		Component(inputName),
		homeAttributes(inputAttributes),
		attributes(homeAttributes)
	{
		projection = std::make_unique<Projection>(attributes.projectionAttributes);
		using namespace std::string_literals;
		transformConstantBuffer = std::make_shared<ConstantBufferInRootSignature<Transforms>>(name + L"#"s + L"CamTransform", 1u);

		projection->SetPos(attributes.position);
		projection->SetRotation(attributes.rotation);

		if (!attributes.isSceneCamera)
		{
			//indicator.SetPos(pos);
		}
	}

	void Camera::BindToGraphics(std::shared_ptr<Camera> camera) const
	{
		Graphics::GetInstance().SetCamera(camera);
	}

	DirectX::XMMATRIX Camera::GetMatrix() const noexcept
	{
		using namespace dx;

		const dx::XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		// apply the camera rotations to a base vector
		const auto lookVector = XMVector3Transform(forwardBaseVector,
			XMMatrixRotationRollPitchYaw(attributes.rotation.x, attributes.rotation.y, attributes.rotation.z)
		);
		// generate camera transform (applied to all objects to arrange them relative
		// to camera position/orientation in world) from cam position and direction
		// camera "top" always faces towards +Y (cannot do a barrel roll)
		const auto camPosition = XMLoadFloat3(&attributes.position);
		const auto camTarget = camPosition + lookVector;

		XMVECTOR upVector;
		if (-PI / 2.0f >= attributes.rotation.x || attributes.rotation.x >= PI / 2.0f)
		{
			upVector = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
		}
		else
		{
			upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		}
		return XMMatrixLookAtLH(camPosition, camTarget, upVector);
	}

	DirectX::XMMATRIX Camera::GetProjection() const noexcept
	{
		return projection->GetMatrix();
	}

	DirectX::XMMATRIX Camera::GetProjection(UINT renderTargetWidth, UINT renderTargetHeight) const noexcept
	{
		return projection->GetMatrix(renderTargetWidth, renderTargetHeight);
	}

	float Camera::GetFOV() const noexcept
	{
		return projection->GetFOV();
	}

	void Camera::Reset() noexcept
	{
		attributes = homeAttributes;

		projection->SetPos(attributes.position);
		projection->SetRotation(attributes.rotation);
		projection->Reset();

		//indicator.SetPos(pos);
		//indicator.SetRotation(homeRot);
	}

	void Camera::Rotate(const float dx, const float dy) noexcept
	{
		attributes.rotation.y = wrap_angle(attributes.rotation.y + dx * rotationSpeed);

		// no limition for pitch
		attributes.rotation.x = wrap_angle(attributes.rotation.x + dy * rotationSpeed);

		//indicator.SetRotation(rotation);
		//proj.SetRotation(rotation);
	}

	void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
	{
		dx::XMStoreFloat3(&translation, dx::XMVector3Transform(
			dx::XMLoadFloat3(&translation),
			dx::XMMatrixRotationRollPitchYaw(attributes.rotation.x, attributes.rotation.y, attributes.rotation.z) *
			dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
		));

		attributes.position = {
			attributes.position.x + translation.x,
			attributes.position.y + translation.y,
			attributes.position.z + translation.z
		};

		//indicator.SetPos(position);
		//proj.SetPos(position);
	}

	DirectX::XMFLOAT3 Camera::GetPos() const noexcept
	{
		return attributes.position;
	}

	void Camera::SetPos(const DirectX::XMFLOAT3& inputPos) noexcept
	{
		attributes.position = inputPos;
		//indicator.SetPos(pos);
		//proj.SetPos(pos);
	}

	//void Camera::LinkTechniques(Rgph::RenderGraph& rg)
	//{
	//	indicator.LinkTechniques(rg);
	//	proj.LinkTechniques(rg);
	//}

	//void Camera::Submit(size_t channels) const
	//{
	//	if (enableCameraIndicator)
	//	{
	//		indicator.Submit(channels);
	//	}
	//	if (enableFrustumIndicator)
	//	{
	//		proj.Submit(channels);
	//	}
	//}

	void Camera::LookZero(const DirectX::XMFLOAT3 inputPos) noexcept
	{
		DirectX::XMFLOAT3 delta = { 
			attributes.position.x - inputPos.x,
			attributes.position.y - inputPos.y,
			attributes.position.z - inputPos.z };
		attributes.rotation.x = wrap_angle(atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)));
		attributes.rotation.y = wrap_angle(atan2(delta.x, delta.z) + PI);
	}

	void Camera::KeepLookFront(const DirectX::XMFLOAT3 inputPos) noexcept
	{
		DirectX::XMFLOAT3 delta = {
			attributes.position.x - inputPos.x,
			attributes.position.y - inputPos.y,
			attributes.position.z - inputPos.z };
		if (-PI / 2.0f >= attributes.rotation.x || attributes.rotation.x >= PI / 2.0f)
		{
			if (0.3f * PI < abs(attributes.rotation.y - wrap_angle(atan2(delta.x, delta.z))) && abs(attributes.rotation.y - wrap_angle(atan2(delta.x, delta.z))) < 0.9 * PI * 2)
			{
				attributes.rotation.x = wrap_angle(atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)));
				attributes.rotation.y = wrap_angle(atan2(delta.x, delta.z) + PI);
			}
			else
			{
				attributes.rotation.x = wrap_angle(-atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)) - PI);
				attributes.rotation.y = wrap_angle(atan2(delta.x, delta.z));
			}
		}
		else
		{
			if (0.3f * PI < abs(attributes.rotation.y - wrap_angle(atan2(delta.x, delta.z) + PI)) && abs(attributes.rotation.y - wrap_angle(atan2(delta.x, delta.z) + PI)) < 0.9f * PI * 2)
			{
				attributes.rotation.x = wrap_angle(-atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)) - PI);
				attributes.rotation.y = wrap_angle(atan2(delta.x, delta.z));
			}
			else
			{
				attributes.rotation.x = wrap_angle(atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)));
				attributes.rotation.y = wrap_angle(atan2(delta.x, delta.z) + PI);
			}
		}
	}

	void Camera::CalculateTransformMatrices() noexcept
	{
		transforms.matrix_V = DirectX::XMMatrixTranspose(GetMatrix());
		transforms.matrix_P = DirectX::XMMatrixTranspose(GetProjection());
		transforms.matrix_VP = transforms.matrix_P * transforms.matrix_V;
		transforms.matrix_I_V = DirectX::XMMatrixInverse(nullptr, transforms.matrix_V);
		transforms.matrix_I_P = DirectX::XMMatrixInverse(nullptr, transforms.matrix_P);
		transforms.matrix_I_VP = DirectX::XMMatrixInverse(nullptr, transforms.matrix_VP);
	}

	void Camera::RotateAround(const float dx, const float dy, const DirectX::XMFLOAT3 centralPoint) noexcept
	{
		using namespace DirectX;
		// Rotate camera around a point
		{
			DirectX::XMFLOAT3 lookVector, destination;
			XMVECTOR rotateVector = XMVectorSubtract(XMLoadFloat3(&attributes.position), XMLoadFloat3(&centralPoint));
			XMStoreFloat3(&lookVector, XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMMatrixRotationRollPitchYaw(attributes.rotation.x, attributes.rotation.y, attributes.rotation.z)));
			XMFLOAT3 finalRatationViewVector;
			XMStoreFloat3(&finalRatationViewVector, XMVector3Transform(rotateVector,
				XMMatrixTranslation(lookVector.x, lookVector.y, lookVector.z) *
				XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
					XMMatrixRotationRollPitchYaw(0.0f, attributes.rotation.y, 0.0f)), dy * rotationSpeed))
				* XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), dx * rotationSpeed))
			));
			XMStoreFloat3(&destination,
				XMVector3Transform(XMLoadFloat3(&centralPoint), XMMatrixTranslation(finalRatationViewVector.x, finalRatationViewVector.y, finalRatationViewVector.z)));
			XMFLOAT3 finalRatationVector;
			XMStoreFloat3(&finalRatationVector, XMVector3Transform(rotateVector,
				XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
					XMMatrixRotationRollPitchYaw(0.0f, attributes.rotation.y, 0.0f)), dy * rotationSpeed))
				* XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), dx * rotationSpeed))
			));
			XMStoreFloat3(&attributes.position,
				XMVector3Transform(XMLoadFloat3(&centralPoint), XMMatrixTranslation(finalRatationVector.x, finalRatationVector.y, finalRatationVector.z)));
			KeepLookFront(destination);
		}
	}

	void Camera::Bind() noxnd
	{
		//using namespace dx;
		//const dx::XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		// apply the camera rotations to a base vector
		//dx::XMFLOAT3 lookVector;
		//XMStoreFloat3(&lookVector, XMVector3Transform(forwardBaseVector,
		//	XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)
		//));
		//DirectX::XMFLOAT4 vWBasisX;
		//DirectX::XMFLOAT4 vWBasisY;
		//DirectX::XMFLOAT4 vWBasisZ;
		//DirectX::XMFLOAT2 UVToViewA;
		//DirectX::XMFLOAT2 UVToViewB;
		//ProjectScreenToWorldExpansionBasis(vWBasisX, vWBasisY, vWBasisZ, UVToViewA, UVToViewB);
		//CameraCBuf cbData = { pos,lookVector,proj.GetFarNearPlane(),vWBasisX,vWBasisY,vWBasisZ,UVToViewA,UVToViewB };
		//vCbuf.Update(gfx, cbData);
		//vCbuf.Bind(gfx);
		//pCbuf.Update(gfx, cbData);
		//pCbuf.Bind(gfx);
		//const_cast<Camera*>(this)->proj.UpdateScreenResolution(gfx);

		CalculateTransformMatrices();
		transformConstantBuffer->Update(transforms);
		transformConstantBuffer->Bind();
	}

	void Camera::SetRotation(const float pitch, const float yaw) noexcept
	{
		attributes.rotation.x = pitch;
		attributes.rotation.y = yaw;

		//indicator.SetRotation(rotation);
		//proj.SetRotation(rotation);
	}

	//void Camera::ProjectScreenToWorldExpansionBasis(dx::XMFLOAT4& vWBasisX, dx::XMFLOAT4& vWBasisY, dx::XMFLOAT4& vWBasisZ,
	//	dx::XMFLOAT2& UVToViewA, dx::XMFLOAT2& UVToViewB) const noxnd
	//{
	//	DirectX::XMFLOAT4X4 cameraToWorld;
	//	dx::XMStoreFloat4x4(&cameraToWorld, DirectX::XMMatrixInverse(nullptr, GetMatrix()));

	//	float FOV = proj.GetFOV();
	//	float farPlane = proj.GetFarNearPlane().x;

	//	dx::XMFLOAT3 vX = { cameraToWorld._11,cameraToWorld._12,cameraToWorld._13 };
	//	dx::XMFLOAT3 vY = { -cameraToWorld._21,-cameraToWorld._22,-cameraToWorld._23 };
	//	dx::XMFLOAT3 vZ = { cameraToWorld._31,cameraToWorld._32,cameraToWorld._33 };
	//	auto NormalizeFloat3 = [](dx::XMFLOAT3& float3)
	//	{
	//		float length = std::sqrt(float3.x * float3.x + float3.y * float3.y + float3.z * float3.z);
	//		float3 = { float3.x / length, float3.y / length, float3.z / length };
	//	};
	//	NormalizeFloat3(vX);
	//	NormalizeFloat3(vY);
	//	NormalizeFloat3(vZ);

	//	float HalfTanFovY = std::tan(FOV * 0.5f);
	//	float HalfTanFovX = proj.GetAspect() * HalfTanFovY;

	//	float tanHalfFovToPlaneY = farPlane * HalfTanFovY;
	//	float tanHalfFovToPlaneX = proj.GetAspect() * tanHalfFovToPlaneY;

	//	vY = { vY.x * tanHalfFovToPlaneY,vY.y * tanHalfFovToPlaneY,vY.z * tanHalfFovToPlaneY };
	//	vX = { vX.x * tanHalfFovToPlaneX,vX.y * tanHalfFovToPlaneX,vX.z * tanHalfFovToPlaneX };
	//	vZ = { vZ.x * farPlane			,vZ.y * farPlane		  ,vZ.z * farPlane };

	//	// remapping for input in range (x:[0, 1], y:[0, 1], z:[0,1])
	//	vZ = { vZ.x - vX.x - vY.x,vZ.y - vX.y - vY.y,vZ.z - vX.z - vY.z };
	//	vX = { vX.x * 2,vX.y * 2,vX.z * 2 };
	//	vY = { vY.x * 2,vY.y * 2,vY.z * 2 };

	//	vWBasisX.x = vX.x; vWBasisX.y = vX.y; vWBasisX.z = vX.z; vWBasisX.w = 0.0f;
	//	vWBasisY.x = vY.x; vWBasisY.y = vY.y; vWBasisY.z = vY.z; vWBasisY.w = 0.0f;
	//	vWBasisZ.x = vZ.x; vWBasisZ.y = vZ.y; vWBasisZ.z = vZ.z; vWBasisZ.w = 0.0f;

	//	UVToViewA.x = -2 * HalfTanFovX; UVToViewA.y = -2 * -1 * HalfTanFovY;
	//	UVToViewB.x = HalfTanFovX; UVToViewB.y = -1 * HalfTanFovY;
	//}

	void Camera::SetOffsetPixels(const float offsetX, const float offsetY) noxnd
	{
		projection->SetOffsetPixels(offsetX, offsetY);
	}

	void Camera::ResizeAspectRatio() noexcept
	{
		ResizeAspectRatio(Graphics::GetInstance().GetWidth(), Graphics::GetInstance().GetHeight());
	}

	void Camera::ResizeAspectRatio(const UINT width, const UINT height) noexcept
	{
		projection->ResizeAspectRatio(width, height);
	}

	Camera::Transforms Camera::GetTransforms() const noexcept
	{
		return transforms;
	}

	void DiveBomber::DEComponent::Camera::DrawComponentUI()
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::CollapsingHeader("Camera"))
		{
			bool rotDirty = false;
			bool posDirty = false;
			const auto dcheck = [](bool d, bool& carry) { carry = carry || d; };
			ImGui::Text("Position");
			dcheck(ImGui::SliderFloat("X", &attributes.position.x, -80.0f, 80.0f, "%.1f"), posDirty);
			dcheck(ImGui::SliderFloat("Y", &attributes.position.y, -80.0f, 80.0f, "%.1f"), posDirty);
			dcheck(ImGui::SliderFloat("Z", &attributes.position.z, -80.0f, 80.0f, "%.1f"), posDirty);
			ImGui::Text("Orientation");
			dcheck(ImGui::SliderAngle("Pitch", &attributes.rotation.x, 0.995f * -90.0f, 0.995f * 90.0f), rotDirty);
			dcheck(ImGui::SliderAngle("Yaw", &attributes.rotation.y, -180.0f, 180.0f), rotDirty);
			// dcheck(ImGui::SliderAngle("Roll", &attributes.rotation.z, -180.0f, 180.0f), rotDirty);
			projection->DrawComponentUI();
			if (!attributes.isSceneCamera)
			{
				ImGui::Checkbox("Camera Indicator", &enableCameraIndicator);
				ImGui::Checkbox("Frustum Indicator", &enableFrustumIndicator);
			}
			if (ImGui::Button("Reset"))
			{
				Reset();
			}

			if (rotDirty)
			{
				const dx::XMFLOAT3 angles = { attributes.rotation.x,attributes.rotation.y,attributes.rotation.z };

				// indicator.SetRotation(angles);
				projection->SetRotation(angles);
			}
			if (posDirty)
			{
				// indicator.SetPos(attributes.position);
				projection->SetPos(attributes.position);
			}
		}
	}
}
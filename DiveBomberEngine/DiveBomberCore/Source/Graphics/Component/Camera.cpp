#include "Camera.h"

#include "..\Graphics.h"
#include "..\BindObj\ConstantBuffer.h"
//#include "imgui/imgui.h"

namespace DiveBomber::Component
{
	using namespace DEGraphics;
	using namespace Utility;
	namespace dx = DirectX;

	Camera::Camera(Graphics& gfx, std::string inputName, CameraAttributes attributes, bool inputTethered) noexcept
		:
		name(std::move(inputName)),
		homePos(attributes.position),
		homeRot(attributes.rotation),
		//indicator(gfx),
		tethered(inputTethered)
		//vCbuf(gfx, 1u),
		//pCbuf(gfx, 1u),
	{
		projection = std::make_unique<Projection>(gfx, attributes.projectionAttributes);

		if (tethered)
		{
			position = homePos;
			rotation = homeRot;
			//indicator.SetPos(pos);
			//proj.SetPos(pos);
		}
		Reset(gfx);
	}

	void Camera::BindToGraphics(DEGraphics::Graphics& gfx, std::shared_ptr<Camera> camera) const
	{
		gfx.SetCamera(camera);
		//gfx.SetProjection(proj.GetMatrix());
		//gfx.SetFOV(proj.GetFOV());
	}

	DirectX::XMMATRIX Camera::GetMatrix() const noexcept
	{
		using namespace dx;

		const dx::XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		// apply the camera rotations to a base vector
		const auto lookVector = XMVector3Transform(forwardBaseVector,
			XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)
		);
		// generate camera transform (applied to all objects to arrange them relative
		// to camera position/orientation in world) from cam position and direction
		// camera "top" always faces towards +Y (cannot do a barrel roll)
		const auto camPosition = XMLoadFloat3(&position);
		const auto camTarget = camPosition + lookVector;

		XMVECTOR upVector;
		if (-PI / 2.0f >= rotation.x || rotation.x >= PI / 2.0f)
		{
			upVector = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
		}
		else
		{
			upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		}
		return XMMatrixLookAtLH(camPosition, camTarget, upVector);
	}

	DirectX::XMMATRIX Camera::GetProjection(DEGraphics::Graphics& gfx) const noexcept
	{
		return projection->GetMatrix(gfx);
	}

	DirectX::XMMATRIX Camera::GetProjection(UINT renderTargetWidth, UINT renderTargetHeight) const noexcept
	{
		return projection->GetMatrix(renderTargetWidth, renderTargetHeight);
	}

	float Camera::GetFOV() const noexcept
	{
		return projection->GetFOV();
	}

	//void Camera::SpawnControlWidgets(Graphics& gfx) noexcept
	//{
	//	bool rotDirty = false;
	//	bool posDirty = false;
	//	const auto dcheck = [](bool d, bool& carry) { carry = carry || d; };
	//	if (!tethered)
	//	{
	//		ImGui::Text("Position");
	//		dcheck(ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f"), posDirty);
	//		dcheck(ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f"), posDirty);
	//		dcheck(ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f"), posDirty);
	//	}
	//	ImGui::Text("Orientation");
	//	dcheck(ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f), rotDirty);
	//	dcheck(ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f), rotDirty);
	//	proj.RenderWidgets(gfx);
	//	ImGui::Checkbox("Camera Indicator", &enableCameraIndicator);
	//	ImGui::Checkbox("Frustum Indicator", &enableFrustumIndicator);
	//	if (ImGui::Button("Reset"))
	//	{
	//		Reset(gfx);
	//	}

	//	if (rotDirty)
	//	{
	//		yaw_ = yaw;
	//		const dx::XMFLOAT3 angles = { pitch,yaw,0.0f };
	//		indicator.SetRotation(angles);
	//		proj.SetRotation(angles);
	//	}
	//	if (posDirty)
	//	{
	//		indicator.SetPos(pos);
	//		proj.SetPos(pos);
	//	}
	//}

	void Camera::Reset(const Graphics& gfx) noexcept
	{
		if (!tethered)
		{
			position = homePos;
			//indicator.SetPos(pos);
			//proj.SetPos(pos);
		}
		rotation = homeRot;
		yaw_ = homeRot.y;

		//indicator.SetRotation(homeRot);
		//proj.SetRotation(homeRot);
		//proj.Reset(gfx);
	}

	void Camera::Rotate(const float dx, const float dy) noexcept
	{
		rotation.y = wrap_angle(rotation.y + dx * rotationSpeed);
		//pitch = std::clamp( pitch + dy * rotationSpeed,0.995f * -PI / 2.0f,0.995f * PI / 2.0f );
		rotation.x = wrap_angle(rotation.x + dy * rotationSpeed);
		yaw_ = rotation.y;
		//indicator.SetRotation(rotation);
		//proj.SetRotation(rotation);
	}

	void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
	{
		if (!tethered)
		{
			dx::XMStoreFloat3(&translation, dx::XMVector3Transform(
				dx::XMLoadFloat3(&translation),
				dx::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
				dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
			));
			position = {
				position.x + translation.x,
				position.y + translation.y,
				position.z + translation.z
			};
			//indicator.SetPos(position);
			//proj.SetPos(position);
		}
	}

	DirectX::XMFLOAT3 Camera::GetPos() const noexcept
	{
		return position;
	}

	void Camera::SetPos(const DirectX::XMFLOAT3& inputPos) noexcept
	{
		position = inputPos;
		//indicator.SetPos(pos);
		//proj.SetPos(pos);
	}

	const std::string& Camera::GetName() const noexcept
	{
		return name;
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
		DirectX::XMFLOAT3 delta = { position.x - inputPos.x,position.y - inputPos.y, position.z - inputPos.z };
		rotation.x = wrap_angle(atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)));
		rotation.y = wrap_angle(atan2(delta.x, delta.z) + PI);
		yaw_ = rotation.y;
	}

	void Camera::KeepLookFront(const DirectX::XMFLOAT3 inputPos) noexcept
	{
		DirectX::XMFLOAT3 delta = { position.x - inputPos.x,position.y - inputPos.y, position.z - inputPos.z };
		if (-PI / 2.0f >= rotation.x || rotation.x >= PI / 2.0f)
		{
			if (0.3 * PI < abs(yaw_ - wrap_angle(atan2(delta.x, delta.z))) && abs(yaw_ - wrap_angle(atan2(delta.x, delta.z))) < 0.9 * PI * 2)
			{
				rotation.x = wrap_angle(atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)));
				rotation.y = wrap_angle(atan2(delta.x, delta.z) + PI);
			}
			else
			{
				rotation.x = wrap_angle(-atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)) - PI);
				rotation.y = wrap_angle(atan2(delta.x, delta.z));
			}
		}
		else
		{
			if (0.3 * PI < abs(yaw_ - wrap_angle(atan2(delta.x, delta.z) + PI)) && abs(yaw_ - wrap_angle(atan2(delta.x, delta.z) + PI)) < 0.9 * PI * 2)
			{
				rotation.x = wrap_angle(-atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)) - PI);
				rotation.y = wrap_angle(atan2(delta.x, delta.z));
			}
			else
			{
				rotation.x = wrap_angle(atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)));
				rotation.y = wrap_angle(atan2(delta.x, delta.z) + PI);
			}
		}
		yaw_ = rotation.y;
	}

	void Camera::RotateAround(const float dx, const float dy, const DirectX::XMFLOAT3 centralPoint) noexcept
	{
		using namespace DirectX;
		// Rotate camera around a point
		{
			DirectX::XMFLOAT3 lookVector, destination;
			XMVECTOR rotateVector = XMVectorSubtract(XMLoadFloat3(&position), XMLoadFloat3(&centralPoint));
			XMStoreFloat3(&lookVector, XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)));
			XMFLOAT3 finalRatationViewVector;
			XMStoreFloat3(&finalRatationViewVector, XMVector3Transform(rotateVector,
				XMMatrixTranslation(lookVector.x, lookVector.y, lookVector.z) *
				XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
					XMMatrixRotationRollPitchYaw(0.0f, rotation.y, 0.0f)), dy * rotationSpeed))
				* XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), dx * rotationSpeed))
			));
			XMStoreFloat3(&destination,
				XMVector3Transform(XMLoadFloat3(&centralPoint), XMMatrixTranslation(finalRatationViewVector.x, finalRatationViewVector.y, finalRatationViewVector.z)));
			XMFLOAT3 finalRatationVector;
			XMStoreFloat3(&finalRatationVector, XMVector3Transform(rotateVector,
				XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
					XMMatrixRotationRollPitchYaw(0.0f, rotation.y, 0.0f)), dy * rotationSpeed))
				* XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), dx * rotationSpeed))
			));
			XMStoreFloat3(&position,
				XMVector3Transform(XMLoadFloat3(&centralPoint), XMMatrixTranslation(finalRatationVector.x, finalRatationVector.y, finalRatationVector.z)));
			KeepLookFront(destination);
		}
	}

	void Camera::Bind(const Graphics& gfx) const noexcept
	{
		using namespace dx;
		const dx::XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
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
	}

	void Camera::SetRotation(const float pitch, const float yaw) noexcept
	{
		rotation.x = pitch;
		rotation.y = yaw;
		yaw_ = rotation.y;
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

	void Camera::ResizeAspectRatio(const Graphics& gfx) noexcept
	{
		ResizeAspectRatio(gfx.GetWidth(), gfx.GetHeight());
	}
	void Camera::ResizeAspectRatio(const UINT width, const UINT height) noexcept
	{
		projection->ResizeAspectRatio(width, height);
	}
}
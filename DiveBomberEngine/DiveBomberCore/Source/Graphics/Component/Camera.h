#pragma once
#include "Projection.h"
//#include "CameraIndicator.h"

#include <DirectXMath.h>
#include <string>

namespace DiveBomber::DEResource
{
	template<typename C>
	class ConstantBufferInRootSignature;
}

namespace DiveBomber::Component
{
	class Camera final
	{
	public:
		struct CameraAttributes
		{
			DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
			DirectX::XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
			Projection::ProjectionAttributes projectionAttributes;
		};

		struct alignas(16) Transforms
		{
			DirectX::XMMATRIX matrix_V;
			DirectX::XMMATRIX matrix_P;
			DirectX::XMMATRIX matrix_VP;
			DirectX::XMMATRIX matrix_I_V;
			DirectX::XMMATRIX matrix_I_P;
			DirectX::XMMATRIX matrix_I_VP;
		};

	public:
		Camera(std::wstring inputName, CameraAttributes attributes, bool inputTethered = false) noexcept;
		Camera(std::wstring inputName, bool tethered = false) noexcept;
		void BindToGraphics(std::shared_ptr<Camera> camera) const;

		[[nodiscard]] DirectX::XMMATRIX GetMatrix() const noexcept;
		[[nodiscard]] DirectX::XMMATRIX GetProjection() const noexcept;
		[[nodiscard]] DirectX::XMMATRIX GetProjection(UINT renderTargetWidth, UINT renderTargetHeight) const noexcept;
		[[nodiscard]] float GetFOV() const noexcept;
		//void SpawnControlWidgets(DEGraphics::Graphics& gfx) noexcept;
		void Reset() noexcept;
		void Rotate(const float dx, const float dy) noexcept;
		void Translate(const DirectX::XMFLOAT3 translation) noexcept;
		[[nodiscard]] DirectX::XMFLOAT3 GetPos() const noexcept;
		void SetPos(const DirectX::XMFLOAT3& inputPos) noexcept;
		[[nodiscard]] const std::wstring& GetName() const noexcept;
		//void LinkTechniques(Rgph::RenderGraph& rg);
		//void Submit(size_t channel) const;

		void LookZero(const DirectX::XMFLOAT3 inputPos) noexcept;
		void RotateAround(const float dx, const float dy, const DirectX::XMFLOAT3 centralPoint) noexcept;
		void Bind() noxnd;
		void SetRotation(const float pitch, const float yaw) noexcept;
		void ProjectScreenToWorldExpansionBasis(DirectX::XMFLOAT4& vWBasisX, DirectX::XMFLOAT4& vWBasisY, DirectX::XMFLOAT4& vWBasisZ,
			DirectX::XMFLOAT2& UVToViewA, DirectX::XMFLOAT2& UVToViewB) const noxnd;
		void SetOffsetPixels(const float offsetX, const float offsetY) noxnd;
		void ResizeAspectRatio() noexcept;
		void ResizeAspectRatio(const UINT width, const UINT height) noexcept;
		[[nodiscard]] Transforms GetTransforms() const noexcept;

	private:
		struct CameraCBuf
		{
			alignas(16) DirectX::XMFLOAT3 pos;
			alignas(16) DirectX::XMFLOAT3 direction;
			alignas(16) DirectX::XMFLOAT2 FNPlane;
			alignas(16) DirectX::XMFLOAT4 vWBasisX;
			alignas(16) DirectX::XMFLOAT4 vWBasisY;
			alignas(16) DirectX::XMFLOAT4 vWBasisZ;
			DirectX::XMFLOAT2 UVToViewA;
			DirectX::XMFLOAT2 UVToViewB;
		};

	private:
		void KeepLookFront(const DirectX::XMFLOAT3 inputPos) noexcept;
		void CalculateTransformMatrices() noexcept;

	private:
		float yaw_;
		std::wstring name;
		bool tethered;
		DirectX::XMFLOAT3 homePos;
		DirectX::XMFLOAT3 homeRot;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotation; //pitch //yaw //roll;
		static constexpr float travelSpeed = 12.0f;
		static constexpr float rotationSpeed = 0.004f;
		bool enableCameraIndicator = false;
		bool enableFrustumIndicator = false;
		std::unique_ptr<Projection> projection;
		//CameraIndicator indicator;

		std::shared_ptr<DEResource::ConstantBufferInRootSignature<Transforms>> transformConstantBuffer;
		Transforms transforms;
	};
}
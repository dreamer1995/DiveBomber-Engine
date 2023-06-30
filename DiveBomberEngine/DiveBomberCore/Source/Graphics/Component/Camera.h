#pragma once
#include "..\Graphics.h"
#include "..\BindObj\ConstantBuffer.h"
#include "Projection.h"
//#include "CameraIndicator.h"

#include <DirectXMath.h>
#include <string>

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

	public:
		Camera(DEGraphics::Graphics& gfx, std::string inputName, CameraAttributes attributes, bool inputTethered = false) noexcept;
		Camera(DEGraphics::Graphics& gfx, std::string inputName, bool tethered = false) noexcept;
		void BindToGraphics(DEGraphics::Graphics& gfx) const;
		DirectX::XMMATRIX GetMatrix() const noexcept;
		DirectX::XMMATRIX GetProjection(DEGraphics::Graphics& gfx) const noexcept;
		//void SpawnControlWidgets(DEGraphics::Graphics& gfx) noexcept;
		void Reset(DEGraphics::Graphics& gfx) noexcept;
		void Rotate(float dx, float dy) noexcept;
		void Translate(DirectX::XMFLOAT3 translation) noexcept;
		DirectX::XMFLOAT3 GetPos() const noexcept;
		void SetPos(const DirectX::XMFLOAT3& inputPos) noexcept;
		const std::string& GetName() const noexcept;
		//void LinkTechniques(Rgph::RenderGraph& rg);
		//void Submit(size_t channel) const;

		void LookZero(DirectX::XMFLOAT3 inputPos) noexcept;
		void RotateAround(float dx, float dy, DirectX::XMFLOAT3 centralPoint) noexcept;
		void Bind(DEGraphics::Graphics& gfx) const noexcept;
		void SetRotation(float pitch, float yaw) noexcept;
		void ProjectScreenToWorldExpansionBasis(DirectX::XMFLOAT4& vWBasisX, DirectX::XMFLOAT4& vWBasisY, DirectX::XMFLOAT4& vWBasisZ,
			DirectX::XMFLOAT2& UVToViewA, DirectX::XMFLOAT2& UVToViewB) const noxnd;
		void SetOffsetPixels(float offsetX, float offsetY) noxnd;

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
		void KeepLookFront(DirectX::XMFLOAT3 inputPos) noexcept;

	private:
		float yaw_;
		std::string name;
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
		// 
		//mutable Bind::VertexConstantBuffer<CameraCBuf> vCbuf;
		//mutable Bind::PixelConstantBuffer<CameraCBuf> pCbuf;
	};
}
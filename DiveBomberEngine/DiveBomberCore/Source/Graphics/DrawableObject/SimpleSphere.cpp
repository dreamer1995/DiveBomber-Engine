#include "SimpleSphere.h"

#include "..\Graphics.h"
#include "..\BindableObject\BindableObjectCommon.h"
#include "..\BindableObject\Geometry\Sphere.h"
#include "..\BindableObject\DynamicConstantBufferInHeap.h"
#include "..\Component\Mesh.h"
#include "..\Component\Material.h"
#include "..\DX\CommandQueue.h"
#include "..\DX\DescriptorAllocator.h"
#include "..\DX\DescriptorAllocation.h"
#include "..\DX\ShaderManager.h"

namespace DiveBomber::DrawableObject
{
	using namespace DEGraphics;
	using namespace DiveBomber::BindableObject;
	using namespace DiveBomber::BindableObject::VertexProcess;
	using namespace DX;
	using namespace Component;
	namespace dx = DirectX;

	SimpleSphere::SimpleSphere(const std::wstring inputName)
	{
		name = inputName;

		VertexLayout vl;
		vl.Append(VertexLayout::Position3D);
		vl.Append(VertexLayout::Normal);
		vl.Append(VertexLayout::Tangent);
		vl.Append(VertexLayout::Binormal);
		vl.Append(VertexLayout::Texture2D);

		IndexedTriangleList sphere = Sphere::MakeNormalUVed(vl, true);
		sphere.Transform(dx::XMMatrixScaling(1, 1, 1));

		const std::string geometryTag = Utility::ToNarrow(name);

		std::shared_ptr<VertexBuffer> vertexBuffer = VertexBuffer::Resolve(geometryTag, sphere.vertices);
		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Resolve(geometryTag, sphere.indices);

		std::shared_ptr<Topology> topology = Topology::Resolve(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		
		mesh = std::make_shared<Mesh>(vertexBuffer, indexBuffer);

		std::shared_ptr<Shader> vertexShader = Shader::Resolve(L"TestShader", ShaderType::VertexShader);
		std::wstring paramsFile = vertexShader->LoadShaderBlob();
		ShaderManager::GetInstance().AddToUsingPool(vertexShader);
		AddBindable(vertexShader);
		std::shared_ptr<Shader> pixelShader = Shader::Resolve(L"TestShader", ShaderType::PixelShader);
		std::wstring paramsFile2 = pixelShader->LoadShaderBlob();
		paramsFile = paramsFile.size() > 0 ? paramsFile : paramsFile2;
		ShaderManager::GetInstance().AddToUsingPool(pixelShader);
		AddBindable(pixelShader);

		std::shared_ptr<RootSignature> rootSignature = RootSignature::Resolve("StandardSRVFullStage");
		AddBindable(rootSignature);

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		auto dsvFormat = DXGI_FORMAT_D32_FLOAT;

		PipelineStateObject::PipelineStateReference pipelineStateReference;
		pipelineStateReference.rootSignature = rootSignature;
		pipelineStateReference.vertexBuffer = vertexBuffer;
		pipelineStateReference.topology = topology;
		pipelineStateReference.vertexShader = vertexShader;
		pipelineStateReference.pixelShader = pixelShader;
		pipelineStateReference.rtvFormats = rtvFormats;
		pipelineStateReference.dsvFormat = dsvFormat;

		std::shared_ptr<PipelineStateObject> pipelineStateObject = PipelineStateObject::Resolve(geometryTag, std::move(pipelineStateReference));
		AddBindable(pipelineStateObject);
		ShaderManager::GetInstance().AddToUsingPool(pipelineStateObject);

		std::shared_ptr<Material> material = std::make_shared<Material>(name + L"Material", paramsFile);
		materialMap.emplace(material->GetName(), material);

		material->SetTexture(Texture::Resolve(L"earth.dds"), 0u);
		material->SetTexture(Texture::Resolve(L"rustediron2_basecolor.png"), 1u);

		std::shared_ptr<ConstantTransformBuffer> transformBuffer = std::make_shared<ConstantTransformBuffer>();
		transformBuffer->InitializeParentReference(*this);
		AddBindable(transformBuffer);
		material->SetConstant(transformBuffer->GetTransformBuffer(), 0u);

		{
			DynamicConstantProcess::RawLayout DCBLayout;
			DCBLayout.Add<DynamicConstantProcess::Float4>("baseColor");
			auto DXBBuffer = DynamicConstantProcess::Buffer(std::move(DCBLayout));
			DXBBuffer["baseColor"] = dx::XMFLOAT4{ 1.0f,0.0f,0.0f,0.0f };
			std::shared_ptr<DynamicConstantBufferInHeap> baseMat = std::make_shared<DynamicConstantBufferInHeap>(geometryTag + "BaseMat0", DXBBuffer);
			material->SetConstant(geometryTag + "BaseMat0", baseMat, 1u);
		}

		{
			DynamicConstantProcess::RawLayout DCBLayout;
			DCBLayout.Add<DynamicConstantProcess::Float4>("baseColor");
			auto DXBBuffer = DynamicConstantProcess::Buffer(std::move(DCBLayout));
			DXBBuffer["baseColor"] = dx::XMFLOAT4{ 0.0f,1.0f,0.0f,0.0f };
			std::shared_ptr<DynamicConstantBufferInHeap> baseMat = std::make_shared<DynamicConstantBufferInHeap>(geometryTag + "BaseMat1", DXBBuffer);
			material->SetConstant(geometryTag + "BaseMat1", baseMat, 2u);
		}
	}

	SimpleSphere::~SimpleSphere()
	{
	}

	void SimpleSphere::SetPos(dx::XMFLOAT3 inputPosition) noexcept
	{
		position = inputPosition;
	}

	void SimpleSphere::SetRotation(dx::XMFLOAT3 inputRotation) noexcept
	{
		pitch = inputRotation.x;
		yaw = inputRotation.y;
		roll = inputRotation.z;
	}
	
	dx::XMMATRIX SimpleSphere::GetTransformXM() const noexcept
	{
		return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			dx::XMMatrixTranslation(position.x, position.y, position.z);
	}

	std::shared_ptr<Material> SimpleSphere::GetMaterialByName(std::wstring name) const noexcept
	{
		auto it = materialMap.find(name);
		if (it != materialMap.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}

	void SimpleSphere::Bind() const noxnd
	{
		mesh->Bind();
		Drawable::Bind();
		for (auto& material : materialMap)
		{
			material.second->Bind();
			Graphics::GetInstance().GetGraphicsCommandList()->DrawIndexedInstanced(mesh->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
		}
	}
}
#include "RootSignature.h"

#include "..\..\DiveBomberCore.h"
#include "..\Graphics.h"
#include "GlobalBindableManager.h"
#include "..\..\Exception\GraphicsException.h"

namespace DiveBomber::BindableObject
{
	using namespace DEGraphics;
	using namespace DEException;

	RootSignature::RootSignature(const std::string& inputTag)
		:
		tag(inputTag)
	{
		HRESULT hr;
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(Graphics::GetInstance().GetDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Allow input layout and deny unnecessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
			D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

		// A single 32-bit constant root parameter that is used by the vertex shader.
		CD3DX12_ROOT_PARAMETER1 rootParameters[2]{};
		rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);

		// Samplers
		const CD3DX12_STATIC_SAMPLER_DESC staticSampler{ 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR };

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 1, &staticSampler, rootSignatureFlags);

		// Serialize the root signature.
		wrl::ComPtr<ID3DBlob> rootSignatureBlob;
		wrl::ComPtr<ID3DBlob> errorBlob;
		GFX_THROW_INFO(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
			featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
		// Create the root signature.
		GFX_THROW_INFO(Graphics::GetInstance().GetDevice()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
	}

	wrl::ComPtr<ID3D12RootSignature> RootSignature::GetRootSignature() noexcept
	{
		return rootSignature;
	}

	void RootSignature::Bind() noxnd
	{
		GFX_THROW_INFO_ONLY(Graphics::GetInstance().GetGraphicsCommandList()->SetGraphicsRootSignature(rootSignature.Get()));
	}

	std::shared_ptr<RootSignature> RootSignature::Resolve(const std::string& tag)
	{
		return GlobalBindableManager::Resolve<RootSignature>(tag);
	}

	std::string RootSignature::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(RootSignature).name() + "#"s + tag;
	}

	std::string RootSignature::GetUID() const noexcept
	{
		return GenerateUID(tag);
	}
}
#include "RootSignature.h"

#include "BindableCodex.h"

namespace DiveBomber::BindObj
{
	using namespace DEGraphics;
	using namespace DEException;

	RootSignature::RootSignature(Graphics& gfx, const std::string& inputTag)
	{
		tag = inputTag;
		HRESULT hr;
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(gfx.GetDecive()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Allow input layout and deny unnecessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		// A single 32-bit constant root parameter that is used by the vertex shader.
		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

		// Serialize the root signature.
		wrl::ComPtr<ID3DBlob> rootSignatureBlob;
		wrl::ComPtr<ID3DBlob> errorBlob;
		GFX_THROW_INFO(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
			featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
		// Create the root signature.
		GFX_THROW_INFO(gfx.GetDecive()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
	}

	wrl::ComPtr<ID3D12RootSignature> RootSignature::GetRootSignature() noexcept
	{
		return rootSignature;
	}

	void RootSignature::Bind(Graphics& gfx) noxnd
	{
		GFX_THROW_INFO_ONLY(gfx.GetCommandList()->SetGraphicsRootSignature(rootSignature.Get()));
	}

	std::shared_ptr<RootSignature> RootSignature::Resolve(Graphics& gfx, const std::string& tag)
	{
		return Codex::Resolve<RootSignature>(gfx, tag);
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
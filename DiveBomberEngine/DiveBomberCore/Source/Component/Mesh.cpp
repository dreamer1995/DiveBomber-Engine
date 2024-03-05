#include "Mesh.h"

#include "..\Graphics\Graphics.h"
#include "..\Graphics\GraphicResource\Bindable\IndexBuffer.h"
#include "..\Graphics\GraphicResource\Bindable\Topology.h"
#include "..\Graphics\GraphicResource\Bindable\ConstantBufferInRootSignature.h"
#include "..\Graphics\GraphicResource\ShaderInputable\StructuredBufferInHeap.h"
#include "..\Graphics\DX\GlobalResourceManager.h"

#include <..\imgui\imgui.h>

namespace DiveBomber::DEComponent
{
    using namespace DEGraphics;
    using namespace GraphicResource;
    using namespace GraphicResource::VertexProcess;
    using namespace DX;

    Mesh::Mesh(std::wstring inputName, VertexData& inputVertexbuffer, std::shared_ptr<IndexBuffer> inputIndexBuffer)
        :
        Mesh(inputName, inputVertexbuffer, inputIndexBuffer, GlobalResourceManager::Resolve<Topology>(L"IndexedTriangle", D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE))
    {
    }

    Mesh::Mesh(std::wstring inputName, VertexData& inputVertexbuffer, std::shared_ptr<IndexBuffer> inputIndexBuffer, std::shared_ptr<Topology> inputTopology)
        :
        Component(inputName),
        vertexData(inputVertexbuffer),
        indexBuffer(inputIndexBuffer),
        topology(inputTopology)
    {
        using namespace std::string_literals;
        vertexDataIndexCB = std::make_shared<ConstantBufferInRootSignature<UINT>>(name + L"#"s + L"MeshConstant", 2u);
        vertexBuffer = std::make_shared<StructuredBufferInHeap<char>>(name +L"#"s + L"VertexData", vertexData.Size());
        vertexBuffer->Update(vertexData.GetData(), vertexData.SizeBytes());
        vertexDataIndexCB->Update(vertexBuffer->GetSRVDescriptorHeapOffset());
    }

    void Mesh::SetMesh(VertexData& inputVertexbuffer, std::shared_ptr<IndexBuffer> inputIndexBuffer) noexcept
    {
        vertexData = inputVertexbuffer;
        indexBuffer = inputIndexBuffer;

        vertexBuffer->Update(vertexData.GetData(), vertexData.SizeBytes());
        vertexBuffer->SetNumElements(vertexData.Size());
        vertexDataIndexCB->Update(vertexBuffer->GetSRVDescriptorHeapOffset());
    }

    void Mesh::SetTopology(std::shared_ptr<Topology> inputTopology) noexcept
    {
        topology = inputTopology;
    }

    GraphicResource::VertexProcess::VertexData& Mesh::GetVertexData() noexcept
    {
        return vertexData;
    }

    std::shared_ptr<IndexBuffer> Mesh::GetIndexBuffer() const noexcept
    {
        return indexBuffer;
    }

    std::shared_ptr<Topology> Mesh::GetTopology() const noexcept
    {
        return topology;
    }

    std::wstring Mesh::GetName() const noexcept
    {
        return name;
    }

    void Mesh::DrawComponentUI()
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::CollapsingHeader("Mesh"))
        {
            ImGui::Text(Utility::ToNarrow(name).c_str());
        }
    }

    void Mesh::Bind() noxnd
    {
        vertexDataIndexCB->Bind();
        indexBuffer->Bind();
        topology->Bind();
    }
}
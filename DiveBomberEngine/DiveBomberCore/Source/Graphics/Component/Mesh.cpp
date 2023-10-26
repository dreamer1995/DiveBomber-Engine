#include "Mesh.h"

#include "..\Graphics.h"
#include "..\BindableObject\IndexBuffer.h"
#include "..\BindableObject\Topology.h"
#include "..\BindableObject\ShaderBuffer\ConstantBuffer.h"
#include "..\BindableObject\ShaderBuffer\StructuredBufferInHeap.h"

namespace DiveBomber::Component
{
    using namespace DEGraphics;
    using namespace BindableObject;

    Mesh::Mesh(std::wstring inputName, BindableObject::VertexProcess::VertexData& inputVertexbuffer, std::shared_ptr<IndexBuffer> inputIndexBuffer)
        :
        Mesh(inputName, inputVertexbuffer, inputIndexBuffer, Topology::Resolve(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE))
    {
    }

    Mesh::Mesh(std::wstring inputName, BindableObject::VertexProcess::VertexData& inputVertexbuffer, std::shared_ptr<IndexBuffer> inputIndexBuffer, std::shared_ptr<Topology> inputTopology)
        :
        name(inputName),
        vertexData(inputVertexbuffer),
        indexBuffer(inputIndexBuffer),
        topology(inputTopology)
    {
        using namespace std::string_literals;
        vertexDataConstantBuffer = std::make_shared<ConstantBuffer<UINT>>(Utility::ToNarrow(name) + "#"s + "MeshConstant", 2u);
        vertexBuffer = std::make_shared<StructuredBufferInHeap<char>>(Utility::ToNarrow(name) +"#"s + "VertexData", vertexData.Size());
        vertexBuffer->Update(vertexData.GetData(), vertexData.SizeBytes());
        vertexDataConstantBuffer->Update(vertexBuffer->GetCBVDescriptorHeapOffset());
    }

    void Mesh::SetMesh(BindableObject::VertexProcess::VertexData& inputVertexbuffer, std::shared_ptr<IndexBuffer> inputIndexBuffer) noexcept
    {
        vertexData = inputVertexbuffer;
        indexBuffer = inputIndexBuffer;

        vertexBuffer->Update(vertexData.GetData(), vertexData.SizeBytes());
        vertexBuffer->SetNumElements(vertexData.Size());
        vertexDataConstantBuffer->Update(vertexBuffer->GetCBVDescriptorHeapOffset());
    }

    void Mesh::SetTopology(std::shared_ptr<Topology> inputTopology) noexcept
    {
        topology = inputTopology;
    }

    BindableObject::VertexProcess::VertexData& Mesh::GetVertexData() noexcept
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

    void Mesh::Bind() noxnd
    {
        vertexDataConstantBuffer->Bind();
        indexBuffer->Bind();
        topology->Bind();
    }
}
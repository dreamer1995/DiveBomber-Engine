#include "Mesh.h"

#include "..\Graphics.h"
#include "..\BindableObject\VertexBuffer.h"
#include "..\BindableObject\IndexBuffer.h"
#include "..\BindableObject\Topology.h"

namespace DiveBomber::Component
{
    using namespace DEGraphics;
    using namespace BindableObject;

    Mesh::Mesh(std::wstring inputName, std::shared_ptr<VertexBuffer> inputVertexbuffer, std::shared_ptr<IndexBuffer> inputIndexBuffer)
        :
        Mesh(inputName, inputVertexbuffer, inputIndexBuffer, Topology::Resolve(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE))
    {
    }

    Mesh::Mesh(std::wstring inputName, std::shared_ptr<VertexBuffer> inputVertexbuffer, std::shared_ptr<IndexBuffer> inputIndexBuffer, std::shared_ptr<Topology> inputTopology)
        :
        name(inputName),
        vertexBuffer(inputVertexbuffer),
        indexBuffer(inputIndexBuffer),
        topology(inputTopology)
    {
    }

    void Mesh::SetMesh(std::shared_ptr<VertexBuffer> inputVertexbuffer, std::shared_ptr<IndexBuffer> inputIndexBuffer) noexcept
    {
        vertexBuffer = inputVertexbuffer;
        indexBuffer = inputIndexBuffer;
    }

    void Mesh::SetTopology(std::shared_ptr<Topology> inputTopology) noexcept
    {
        topology = inputTopology;
    }

    std::shared_ptr<VertexBuffer> Mesh::GetVertexBuffer() const noexcept
    {
        return vertexBuffer;
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
        //vertexBuffer->Bind();
        indexBuffer->Bind();
        topology->Bind();
    }
}
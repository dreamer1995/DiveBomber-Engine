#include "Prefab.h"

#include "..\..\Exception\GraphicsException.h"

namespace DiveBomber::DEObject
{
    using namespace DEException;

    Prefab::Prefab(const fs::path inputPath)
        :
        Object(inputPath)
    {
    }

    void Prefab::DrawDetailPanel()
    {
    }
    void Prefab::CreateConfig()
    {
    }
    void Prefab::SaveConfig()
    {
    }
    void Prefab::UpdateConfig() noexcept
    {
    }
}
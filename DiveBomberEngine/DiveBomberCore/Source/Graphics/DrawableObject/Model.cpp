#include "Model.h"

#include "..\..\Exception\GraphicsException.h"

namespace DiveBomber::DrawableObject
{
    using namespace DEGraphics;
    using namespace DEException;

    Model::Model()
    {
    }

    int Model::GetModel() const noexcept
    {
        return 0;
    }

    void Model::Bind(Graphics& gfx) noxnd
    {

    }
}
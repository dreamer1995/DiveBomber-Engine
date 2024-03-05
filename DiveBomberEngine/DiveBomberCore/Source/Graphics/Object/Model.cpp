#include "Model.h"

#include "..\..\Exception\GraphicsException.h"

namespace DiveBomber::DEObject
{
    using namespace DEException;

    Model::Model(const std::wstring inputName)
        :
        Object(inputName)
    {
    }

    int Model::GetModel() const noexcept
    {
        return 0;
    }

    void Model::Bind() noxnd
    {

    }
}
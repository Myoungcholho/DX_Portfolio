#pragma once

#include "BillboardData.h"

class IBillboardRenderer
{
public:
    virtual ~IBillboardRenderer() = default;
    virtual void Initialize() = 0;
    virtual void Tick() {}
    virtual void Render(BillboardInstance* data) = 0;
};
#pragma once

#include "scene/component/component.hpp"
#include "io/logging/logger.hpp"

SIERRA_COMP(Foo)
    public:
    Foo();
    void update() override;
};
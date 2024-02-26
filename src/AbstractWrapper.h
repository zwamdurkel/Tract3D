#pragma once

#include "RenderSettings.h"

class AbstractWrapper {
public:
    explicit AbstractWrapper() = default;

    // Initialize everything needed to use this tool.
    virtual void init() = 0;

    // Use the tool in the render loop.
    virtual void use() = 0;

    // Clean up resources before exiting.
    virtual void cleanup() = 0;

protected:
    RenderSettings& settings = RenderSettings::getInstance();
};
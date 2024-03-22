#pragma once

class RenderSettings;

class AbstractWrapper {
public:
    explicit AbstractWrapper() = default;

    // Initialize everything needed to use this tool.
    virtual void init() = 0;

    // Use the tool in the render loop.
    virtual void draw() = 0;

    // Clean up resources before exiting.
    virtual void cleanup() = 0;

protected:
    static RenderSettings& settings;
};
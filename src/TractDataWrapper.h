#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "AbstractWrapper.h"

struct Tract {
    std::vector<float> vertices;
    std::vector<float> gradient;
};

class TractDataWrapper : AbstractWrapper {

private:
    //helper function that returns next line of file separated by spaces in vector of strings
    std::vector<std::string> readline(std::ifstream& file);

public:
    std::vector<Tract> data;
    // The number of vertices in each tract.
    std::vector<int> tractSizes;
    std::vector<unsigned int> tractIndices;
    // tractEndIndex[i] stores the index of the last vertex of the i'th tract in tractIndices
    std::vector<int> tractEndIndex;
    // How many out of `tractCount` tracts do we render?
    int showTractCount = 1;
    // Total number of tracts represented by this class.
    int tractCount = 1;
    unsigned int VAO, VBO, VCO, GVO, EBO;

    float rainbow[21] = {
            1.0f, 0.15f, 0.15f, // red
            1.0f, 0.6f, 0.0f, // orange
            1.0f, 0.95f, 0.0f, // yellow
            0.2f, 1.0f, 0.2f, // green
            0.2f, 0.15f, 0.95f, // blue
            0.45f, 0.15f, 1.0f, //violet
            0.55f, 0.15f, 1.0f //purple
    };

    explicit TractDataWrapper(const char* filePath);

    TractDataWrapper() = default;

    bool parse(const char* filePath, bool tractStop);

    void cleanup() override {}

    void draw() override;

    void init() override;
};
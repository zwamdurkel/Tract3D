#pragma once

#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include "AbstractWrapper.h"

struct Tract {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> gradient;
    std::vector<int> indices;
};

class TractDataWrapper : AbstractWrapper {

private:
    //helper function that returns next line of file separated by spaces in vector of strings
    std::vector<std::string> readline(std::ifstream& file);

    void makeContour(int sides, Tract& t, int i);

    void projectContour(int sides, Tract& t, int i);

    void constructTubes(int sides);

    constexpr static float fixedSin[9] = {0.0f, 0.0f, 0.0f, 0.86602540378f, 1.0f, 0.95105651629f, 0.86602540378f,
                                          0.78183148246f, 0.70710678118f};
    constexpr static float fixedCos[9] = {0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.30901699437f, 0.5f, 0.62348980185f,
                                          0.70710678118f};

public:
    std::string name;
    bool enabled = true;
    float alpha = 1.0f;
    float diameter = 0.1f;
    std::vector<Tract> data;
    // tractEndIndex[i] stores the index of the last vertex of the i'th tract in tractIndices
    std::vector<int> tractEndIndex;
    // How many out of `tractCount` tracts do we render?
    int showTractCount = 1;
    // Total number of tracts represented by this class.
    int tractCount = 1;
    unsigned int VAO, VBO, GVO, EBO, VNO;
    std::vector<glm::vec3> gradients;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;

    explicit TractDataWrapper(std::string name) : name(std::move(name)) {};

    TractDataWrapper(std::string name, const char* filePath);

    TractDataWrapper() = default;

    bool parse(const char* filePath, bool tractStop);

    void cleanup() override {}

    void draw() override;

    void init() override;
};
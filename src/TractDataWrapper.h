#pragma once

#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include "AbstractWrapper.h"
#include "glm/glm.hpp"

struct Tract {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> gradient;
    std::vector<int> indices;
};

struct ssboUnit {
    float position[3];
    float gradient[3];
};

class TractDataWrapper : AbstractWrapper {

private:
    unsigned int VAO = 0, SSBO = 0;
    std::vector<ssboUnit> ssboData;
    Tract avgTract;
    std::vector<float> avgTractWidth;
    std::vector<int32_t> counts;
    std::vector<int32_t> firsts;

    //helper function that returns next line of file separated by spaces in vector of strings
    std::vector<std::string> readline(std::ifstream& file);

    void generateAverageTract(int nrOfPoints = 20);

    void generateTractClassification();

public:
    std::string name;
    bool enabled = true;
    float alpha = 1.0f;
    std::vector<Tract> data;
    // How many out of `tractCount` tracts do we render?
    int showTractCount = 1;
    // Total number of tracts represented by this class.
    int tractCount = 1;

    explicit TractDataWrapper(std::string name) : name(std::move(name)) {};

    TractDataWrapper(std::string name, const std::string& filePath);

    ~TractDataWrapper();

    glm::vec3 getBezierPosition(int t);

    glm::vec3 getBezierDirection(int t);

    bool parse(const std::string& filePath, bool tractStop);

    void cleanup() override {}

    void draw() override;

    void init() override;

    void bindSSBO();

    int getVertexNum() { return ssboData.size(); }
};
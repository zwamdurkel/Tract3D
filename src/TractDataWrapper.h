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

struct bezierPoint {
    glm::vec3 pos;
    glm::vec3 dir;
};

class TractDataWrapper : AbstractWrapper {

private:
    unsigned int VAO = 0, SSBO = 0, DB = 0;
    std::vector<ssboUnit> ssboData;
    Tract avgTract;
    std::vector<float> avgTractWidth;
    std::vector<int32_t> counts;
    std::vector<int32_t> counts2;
    std::vector<int32_t> firsts;
    std::vector<int32_t> endCapCounts;
    std::vector<int32_t> endCapfirsts;
    std::vector<float> displacements;
    std::vector<int> nearestavgVertex;
    int avgFidelity = 10;
    int classificationFidelity = 1;

    //helper function that returns next line of file separated by spaces in vector of strings
    std::vector<std::string> readline(std::ifstream& file);

    void generateAverageTract(int nrOfPoints = 30);

    void generateTractClassification();

    void computeExpandingView();

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

    bezierPoint getBezierPosition(float time, int tractNr);

    void bindDB();

    void clearDB();

    bool parse(const std::string& filePath, bool tractStop);

    void cleanup() override {}

    void draw() override;

    void init() override;

    void bindSSBO();

    std::vector<ssboUnit> getSSBOData() { return ssboData; }

    int getVertexNum() { return ssboData.size(); }
};
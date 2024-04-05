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
    glm::vec3 avgPoint;
    std::vector<int32_t> counts;
    std::vector<int32_t> firsts;
    std::vector<int32_t> capCounts;
    std::vector<int32_t> capFirsts;
    std::vector<glm::vec3> displacements;
    int avgFidelity = 10;

    //helper function that returns next line of file separated by spaces in vector of strings
    std::vector<std::string> readline(std::ifstream& file);

    // Generates an average tract of length `nrOfPoints`. Tracts should be classified for best results
    void generateAverageTract(int nrOfPoints = 30);

    // Makes sure all tracts have their starting points on the same side
    void generateTractClassification();

    // Computes the displacement for each vertex based on the `avgTract`
    void computeExpandingView();

    // Calculate the center point of this dataset and store it in `avgPoint`
    void calculateCenterPoint();

public:
    std::string name;
    bool enabled = true;
    float alpha = 1.0f;
    std::vector<Tract> data;
    // How many out of `tractCount` tracts do we render?
    int showCount = 1;
    // Total number of tracts represented by this class.
    int tractCount = 1;

    explicit TractDataWrapper(std::string name) : name(std::move(name)) {};

    TractDataWrapper(std::string name, const std::string& filePath);

    ~TractDataWrapper();

    // Get a position and direction on a curve along a tract
    bezierPoint getBezierPosition(float time, int tractNr);

    // Update the displacements and write to GPU buffer DB
    void updateDB();

    bool parse(const std::string& filePath, bool tractStop);

    void cleanup() override {}

    void draw() override;

    void init() override;

    void bindSSBO();

    std::vector<ssboUnit> getSSBOData() { return ssboData; }
};
#pragma once

#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include "AbstractWrapper.h"
#include "glm/glm.hpp"

// A tract is a "curve" consisting of joined line segments going through `n` vertices
// A crude representation:
//     v---v---v           v---v
//     |        \         /
// v---v         v---v---v
//
// If a corner is like this, then the gradient is drawn
//   g (45 degrees, the average direction of two line segments)
//  /
// v---v
// |
// v
struct Tract {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> gradient;
    std::vector<int> indices;
};

struct ssboUnit {
    float position[3];
    // Average direction at tract point.
    float gradient[3];
};

struct bezierPoint {
    glm::vec3 pos;
    glm::vec3 dir;
};

class TractDataWrapper : AbstractWrapper {
private:
    unsigned int VAO = 0, SSBO = 0, DB = 0;
    // We store all the tract vertices on the GPU
    std::vector<ssboUnit> ssboData;
    // This tract represents a tract whose coords are the average of the bundle. i.e., it's in the middle of the tract
    Tract avgTract;
    // Point in the middle of the bundle
    glm::vec3 avgPoint;
    std::vector<int32_t> counts;
    std::vector<int32_t> firsts;
    std::vector<int32_t> capCounts;
    std::vector<int32_t> capFirsts;
    // By how much each vertex is displaced if the effect is enabled
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
    // Identifying name for this tract bundle
    std::string name;
    // Whether the tract will be rendered
    bool enabled = true;
    // Transparency of the tract bundle
    float alpha = 1.0f;
    // Full tract data as read from file
    //
    // Structure:
    // data [
    //      tract [ vertices, gradients, indices ],
    //      tract [ ... ],
    //      tract [ ... ],
    //      ...
    // ]
    std::vector<Tract> data;
    // How many out of `tractCount` tracts do we render?
    int showCount = 1;
    // Total number of tracts represented by this class
    int tractCount = 1;

    explicit TractDataWrapper(std::string name) : name(std::move(name)) {
    };

    TractDataWrapper(std::string name, const std::string& filePath);

    ~TractDataWrapper();

    // Get a position and direction on a curve along a tract
    bezierPoint getBezierPosition(float time, int tractNr);

    // Update the displacements and write to GPU buffer DB
    void updateDB();

    // Read a .tck file and store the date in `data`
    bool parse(const std::string& filePath, bool tractStop);

    void cleanup() override {
    }

    void draw() override;

    // Initialize the variables needed for rendering. May be called multiple times to apply new settings
    void init() override;

    std::vector<ssboUnit> getSSBOData() { return ssboData; }
};

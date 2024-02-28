#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "AbstractWrapper.h"

struct Tract {
    std::vector<float> vertices;
};

class TractData : AbstractWrapper {

private:
    //helper function that returns next line of file separated by spaces in vector of strings
    std::vector<std::string> readline(std::ifstream& file);

public:
    std::vector<Tract> data;
    std::vector<int> tractSizes;
    std::vector<unsigned int> tractIndices;
    // tractEndIndex[i] stores the index of the last vertex of the i'th tract in tractIndices
    std::vector<int> tractEndIndex;

    bool parse(const char* filePath, bool tractStop);
    void cleanup() override {}
    void draw() override;
    void init() override;
};
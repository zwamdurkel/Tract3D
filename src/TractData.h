#pragma once

#include <vector>
#include <string>
#include <fstream>

struct Tract {
    std::vector<float> vertices;
};

class TractData {

private:
    //helper function that returns next line of file separated by spaces in vector of strings
    std::vector<std::string> readline(std::ifstream& file);

public:
    std::vector<Tract> data;
    std::vector<int> tractSizes;
    std::vector<int> tractIndices;

    bool parse(const char* filePath, bool tractStop);
};
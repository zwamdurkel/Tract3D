#include "TractData.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "logger.h"

std::vector<std::string> TractData::readline(std::ifstream& file) {
    std::string input;
    getline(file, input);
    std::stringstream ss(input);
    std::string segment;
    std::vector<std::string> strings;

    while (getline(ss, segment, ' ')) {
        strings.push_back(segment);
    }

    return strings;
}

bool TractData::parse(const char* filePath, bool tractStop) {
    std::ifstream file;
    file.open(filePath, std::ios::binary);

    if (!file.is_open()) {
        Error("File did not open");
        return false;
    }

    std::vector<std::string> line;
    int count = -1;
    // read key value pairs until END keyword
    do {
        line = readline(file);
        for (const std::string& i: line)
            Debug("reading: " << i << " ");

        //find number of tracts
        if (line[0] == "count:") {
            try {
                count = std::stoi(line[1]);
            } catch (const std::invalid_argument& ex) {
                Error("Cannot convert count to valid integer");
                return false;
            }
        }
        //check datatype
        if (line[0] == "datatype:" && line[1] != "Float32LE") {
            Error("Unsupported datatype " << line[1]
                                          << ", expected Float32LE, correct data interpretation not guaranteed");
            return false;
        }

    } while (line[0] != "END");

    // rest of file should be binary data
    // tracts separated by (NaN,NaN,NaN)
    // end of binary data is (inf,inf,inf)
    std::vector<Tract> tracts;
    if (count > 0) {
        tracts.reserve(count);
    }
    float f[3];
    Tract t;
    int vertCount = 0;
    int firstCount = 0;
    while (!std::isinf(f[0])) { // read all coordinates from binary data until we encounter (inf,inf,inf)
        file.read((char*) &f, 12);//read float triplet (3*4 = 12 bytes)
        if (std::isnan(f[0])) {
            tractSizes.push_back(vertCount);
            tractIndices.push_back(firstCount-vertCount);
            vertCount = 0;
            if (tractStop) {
                tracts.push_back(t);
                t.vertices.clear();
            } else {
                continue;
            }
        } else {
            vertCount++;
            firstCount++;
            t.vertices.push_back(f[0]);
            t.vertices.push_back(f[1]);
            t.vertices.push_back(f[2]);
        }
    }
    if (!tractStop) {
        tracts.push_back(t);
    }
    data = std::move(tracts);
    return true;
}
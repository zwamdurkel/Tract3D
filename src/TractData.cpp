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

void TractData::parse(const char* filePath) {
    std::ifstream file;
    file.open(filePath, std::ios::binary);
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
            }
        }
        //check datatype
        if (line[0] == "datatype:" && line[1] != "Float32LE") {
            Error("unsupported datatype, correct data interpretation not guaranteed");
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
    while (!std::isinf(f[0])) { // read all coordinates from binary data until we encounter (inf,inf,inf)
        file.read((char*) &f, 12);//read float triplet (3*4 = 12 bytes)

        if (std::isnan(f[0])) {
            tracts.push_back(t);
            t.vertices.clear();
        } else {
            t.vertices.insert(t.vertices.end(), f, f + 3);
        }
    }

    data = std::move(tracts);
}
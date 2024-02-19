#include "TractData.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include "logger.h"

std::vector<std::string> TractData::readline(std::ifstream& file) {
    std::string input;
    getline(file, input);
    std::stringstream ss(input);
    std::string segment;
    std::vector<std::string> strings;

    while (getline(ss, segment, ' ')){
        strings.push_back(segment);
    }

    return strings;
}

void TractData::parse(const char *filePath) {
    std::ifstream file;
    file.open(filePath, std::ios::binary);
    std::vector<std::string> line;

    // read key value pairs until END keyword
    do{
        line = readline(file);
        for (int i = 0; i < line.size(); i++)
             Debug("reading: " << line[i] << " ");

        //check datatype
        if (!line[0].compare("datatype:") && line[1].compare("Float32LE")){
            Error("unsupported datatype, correct data interpretation not guaranteed");
        }

    } while (line[0].compare("END"));

    // rest of file should be binary data
    // tracts separated by (NaN,NaN,NaN)
    // end of binary data is (inf,inf,inf)
    std::vector<Tract> tracts;

    float f[3];
    while (!isinf(f[0])){ // read all coordinates from binary data until we encounter (inf,inf,inf)
        Tract t;
        file.read((char*)&f, 12);//read float triplet (3*4 = 12 bytes)

        while (!isnan(f[0])){ // keep adding coordinates to a tract until we encounter (NaN,NaN,NaN)
            if (isinf(f[0])){
                break;
            }

            t.vertices.push_back(f[0]);
            t.vertices.push_back(f[1]);
            t.vertices.push_back(f[2]);

            file.read((char*)&f, 12);//read float triplet (3*4 = 12 bytes)
        }
        tracts.push_back(t);
    }
    data = tracts;
}
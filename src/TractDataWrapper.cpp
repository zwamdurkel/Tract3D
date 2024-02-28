#include "TractDataWrapper.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "logger.h"
#include "path.h"

std::vector<std::string> TractDataWrapper::readline(std::ifstream& file) {
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

bool TractDataWrapper::parse(const char* filePath, bool tractStop) {
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
            tractEndIndex.push_back((int) tractIndices.size());
            // 0xFFFFFFFF is the primitive restart value, i.e, the renderer will see this as the start of a new line.
            tractIndices.push_back(0xFFFFFFFF);
            vertCount = 0;
            if (tractStop) {
                tracts.push_back(t);
                t.vertices.clear();
            } else {
                continue;
            }
        } else {
            tractIndices.push_back(firstCount);
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

    // Set the number of tracts we have
    tractCount = showTractCount = (int) tractEndIndex.size();
    return true;
}

void TractDataWrapper::init() {
    // Use primitive restart to detect beginning of new tract in buffer.
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xFFFFFFFF);

    // Color Array
    std::vector<float> colors;
    for (int i: tractSizes) {
        int first = round(i / (sizeof(rainbow) / sizeof(float) / 3));
        int last = i - (first * (sizeof(rainbow) / sizeof(float) / 3 - 1));
        for (int j = 0; j < (sizeof(rainbow) / sizeof(float) - 3); j += 3) {
            for (int k = 0; k < first; k++) {
                colors.push_back(rainbow[j]);
                colors.push_back(rainbow[j + 1]);
                colors.push_back(rainbow[j + 2]);
            }
        }
        for (int k = 0; k < last; k++) {
            colors.push_back(rainbow[(sizeof(rainbow) / sizeof(float)) - 3]);
            colors.push_back(rainbow[(sizeof(rainbow) / sizeof(float)) - 2]);
            colors.push_back(rainbow[(sizeof(rainbow) / sizeof(float)) - 1]);
        }
    }

    // Generate buffers. VAO = Vertex Array Object, VBO = Vertex Buffer Object, VCO = Vertex Color Object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VCO);

    // Bind Vertex Array Object
    glBindVertexArray(VAO);
    // Copy Vertices Array to a Buffer for OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (long) (data[0].vertices.size() * sizeof(float)), &data[0].vertices[0],
                 GL_STATIC_DRAW);
    // Then set our Vertex Attributes Pointers
    // Position Attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * tractIndices.size(), &tractIndices[0], GL_STATIC_DRAW);
    // Copy Colors Array to a Buffer for OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VCO);
    glBufferData(GL_ARRAY_BUFFER, (long) (colors.size() * sizeof(float)), &colors[0], GL_STATIC_DRAW);
    // Color Attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
}

void TractDataWrapper::draw() {
    glDrawElements(GL_LINE_STRIP, (int) tractEndIndex[showTractCount - 1], GL_UNSIGNED_INT, nullptr);
}

TractDataWrapper::TractDataWrapper(const char* filePath) {
    parse(filePath, false);
    TractDataWrapper::init();
}


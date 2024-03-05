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

    glm::vec3 prev1(0.0f, 0.0f, 0.0f);//one before
    glm::vec3 prev2(0.0f, 0.0f, 0.0f);//two before
    glm::vec3 gradient;

    while (!std::isinf(f[0])) { // read all coordinates from binary data until we encounter (inf,inf,inf)
        file.read((char*) &f, 12);//read float triplet (3*4 = 12 bytes)
        if (std::isnan(f[0])) {
            vertCount = 0;

            //add final gradient
            gradient = glm::normalize(prev1 - prev2);
            t.gradient.emplace_back(gradient.x, gradient.y, gradient.z);
            //reset previous positions when starting a new tract
            prev1 = glm::vec3(0.0f, 0.0f, 0.0f);
            prev2 = glm::vec3(0.0f, 0.0f, 0.0f);

            if (tractStop) {
                tracts.push_back(t);
                t.vertices.clear();
                t.gradient.clear();
                t.indices.clear();
            } else {
                continue;
            }
        } else {
            t.indices.push_back(firstCount);
            vertCount++;
            firstCount++;
            t.vertices.emplace_back(f[0], f[1], f[2]);

            //gradient vector
            glm::vec3 current = glm::vec3(f[0], f[1], f[2]);
            if (glm::distance(prev1, glm::vec3(0.0f, 0.0f, 0.0f)) > 0.01) {
                if (glm::distance(prev2, glm::vec3(0.0f, 0.0f, 0.0f)) > 0.01) {
                    glm::vec3 prevDelta = glm::normalize(prev1 - prev2);
                    glm::vec3 currDelta = glm::normalize(current - prev1);
                    gradient = glm::normalize(prevDelta + currDelta);

                    t.gradient.emplace_back(gradient.x, gradient.y, gradient.z);
                } else {
                    gradient = glm::normalize(current - prev1);
                    t.gradient.emplace_back(gradient.x, gradient.y, gradient.z);
                }
            }
            prev2 = prev1;
            prev1 = current;
        }
    }
    if (!tractStop) {
        tracts.push_back(t);
    }
    data = std::move(tracts);

    // Set the number of tracts we have
    tractCount = showTractCount = (int) data.size();
    return true;
}

// Construct "sides"-sided tubes for the current "data"
void TractDataWrapper::constructTubes(int sides) {
    int capIndices[sides];
    capIndices[0] = 0;
    int offset;

    // Pre-calculate cap indices.
    for (int i = 2; i <= sides; i++) {
        if (i % 2 == 0) {
            capIndices[i - 1] = i / 2;
        } else {
            capIndices[i - 1] = sides - i / 2;
        }
    }

    // For each tract...
    for (auto& t: data) {
        offset = vertices.size();

        // ---------
        // begin cap
        // ---------
        for (int ci: capIndices) {
            indices.push_back((sides - ci) % sides + offset);
        }

        indices.push_back(0xFFFFFFFF);

        // -------------
        // tube segments
        // -------------
        makeContour(sides, t, 0);

        for (int i = 1; i < t.vertices.size(); i++) {
            offset = vertices.size();
            makeContour(sides, t, i);
            indices.push_back(offset - sides);
            indices.push_back(offset);

            for (int j = 1; j <= sides; j++) {
                indices.push_back(offset - j);
                indices.push_back(offset + sides - j);
            }

            indices.push_back(0xFFFFFFFF);
        }

        // -------
        // end cap
        // -------
        offset = vertices.size() - sides;

        for (int ci: capIndices) {
            indices.push_back(ci + offset);
        }

        tractEndIndex.push_back(indices.size());
        indices.push_back(0xFFFFFFFF);
    }
}

// Make a "sides"-sided polygon around vertex "i" in Tract "t" around the gradient.
void TractDataWrapper::makeContour(int sides, Tract& t, int i) {
    const float diameter = 0.1f;
    glm::mat4 rot = glm::rotate(glm::mat4(1), glm::radians(360.0f / (float) sides), t.gradient[i]);
    auto v = glm::normalize(glm::cross(t.gradient[i], glm::vec3(0, 1, 0))) * diameter;
    vertices.push_back(v + t.vertices[i]);
    gradients.push_back(t.gradient[i]);
    normals.push_back(v);

    for (int j = 1; j < sides; j++) {
        v = rot * glm::vec4(normals.back(), 0);
        vertices.push_back(v + t.vertices[i]);
        gradients.push_back(t.gradient[i]);
        normals.push_back(v);
    }
}

void TractDataWrapper::init() {
    // Use primitive restart to detect beginning of new tract in buffer.
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xFFFFFFFF);

    vertices.clear();
    gradients.clear();
    normals.clear();
    indices.clear();
    tractEndIndex.clear();
    if (settings.drawTubes) {
        constructTubes(settings.nrOfSides);
    } else {
        for (auto tract: data) {
            gradients.insert(gradients.end(), tract.gradient.begin(), tract.gradient.end());
            vertices.insert(vertices.end(), tract.vertices.begin(), tract.vertices.end());
            normals.insert(normals.end(), tract.gradient.begin(), tract.gradient.end());
            indices.insert(indices.end(), tract.indices.begin(), tract.indices.end());
            tractEndIndex.push_back(indices.size());
            indices.push_back(0xFFFFFFFF);
        }
    }

    // Bind Vertex Array Object
    glBindVertexArray(VAO);
    // Copy Vertices Array to a Buffer for OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (long) (vertices.size() * sizeof(glm::vec3)), &vertices[0],
                 GL_STATIC_DRAW);
    // Then set our Vertex Attributes Pointers
    // Position Attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Copy Vertices Array to a Buffer for OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, GVO);
    glBufferData(GL_ARRAY_BUFFER, (long) (gradients.size() * sizeof(glm::vec3)), &gradients[0],
                 GL_STATIC_DRAW);
    // Then set our Vertex Attributes Pointers
    // Position Attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // Copy Vertices Array to a Buffer for OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VNO);
    glBufferData(GL_ARRAY_BUFFER, (long) (normals.size() * sizeof(glm::vec3)), &normals[0],
                 GL_STATIC_DRAW);
    // Then set our Vertex Attributes Pointers
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
}

void TractDataWrapper::draw() {
    glBindVertexArray(VAO);
    if (settings.drawTubes) {
        glDrawElements(GL_TRIANGLE_STRIP, (int) tractEndIndex[showTractCount - 1], GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawElements(GL_LINE_STRIP, (int) tractEndIndex[showTractCount - 1], GL_UNSIGNED_INT, nullptr);
    }
}

TractDataWrapper::TractDataWrapper(const char* filePath, std::string n) {
    parse(filePath, true);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &GVO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VNO);
    name = n;
    enabled = true;
    TractDataWrapper::init();
}


#include "TractDataWrapper.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "logger.h"
#include <chrono>
#include "RenderSettings.h"
#include <glm/gtc/type_ptr.hpp>

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

bool TractDataWrapper::parse(const std::string& filePath, bool tractStop) {
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream file;
    file.open(filePath, std::ios::binary);

    if (!file.is_open()) {
        Error("File did not open");
        return false;
    }

    std::vector<std::string> line;
    int count = -1;
    int byteOffset = 0;
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
        if (line[0] == "file:") {
            byteOffset = std::stoi(line[2]);
        }
        //check datatype
        if (line[0] == "datatype:" && line[1] != "Float32LE") {
            Error("Unsupported datatype " << line[1]
                                          << ", expected Float32LE, correct data interpretation not guaranteed");
            return false;
        }

    } while (line[0] != "END");
    // set filestream to required offset
    file.seekg(byteOffset);

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

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    Info("Parsed file " << filePath << " in " << duration.count() << "ms");
    return true;
}

// Construct "sides"-sided tubes for the current "data"
void TractDataWrapper::constructTubes(int sides) {
    std::vector<int> capIndices;
    capIndices.emplace_back(0);
    int offset;

    // Pre-calculate cap indices.
    for (int i = 2; i <= sides; i++) {
        if (i % 2 == 0) {
            capIndices.emplace_back(i / 2);
        } else {
            capIndices.emplace_back(sides - i / 2);
        }
    }

    // For each tract...
    for (auto& t: data) {
        offset = vertices.size();

        // ---------
        // Begin cap
        // ---------
        for (int ci: capIndices) {
            indices.emplace_back((sides - ci) % sides + offset);
        }

        indices.emplace_back(0xFFFFFFFF);

        // -------------
        // Tube segments
        // -------------
        makeContour(sides, t, 0);

        for (int i = 1; i < t.vertices.size(); i++) {
            offset = vertices.size();
            projectContour(sides, t, i);
            indices.emplace_back(offset - sides);
            indices.emplace_back(offset);

            for (int j = 1; j <= sides; j++) {
                indices.emplace_back(offset - j);
                indices.emplace_back(offset + sides - j);
            }

            indices.emplace_back(0xFFFFFFFF);
        }

        // -------
        // End cap
        // -------
        offset = vertices.size() - sides;

        for (int ci: capIndices) {
            indices.emplace_back(ci + offset);
        }

        tractEndIndex.emplace_back(indices.size());
        indices.emplace_back(0xFFFFFFFF);
    }
}

// Make a "sides"-sided polygon around vertex "i" in Tract "t" around the gradient.
void TractDataWrapper::makeContour(int sides, Tract& t, int i) {
    auto& r = t.gradient[i];
    // Vector perpendicular to the line segment
    auto v = glm::normalize(glm::cross(r, glm::vec3(0, 1, 0))) * settings.tubeDiameter;
    addNormalAsByte(v);
    vertices.emplace_back(v + t.vertices[i]);
    addColorAsByte(r);

    // Rodrigues' Rotation Formula: new v := rotate old v around r
    for (int j = 1; j < sides; j++) {
        v = (1 - fixedCos[sides]) * glm::dot(v, r) * r
            + fixedCos[sides] * v
            + fixedSin[sides] * cross(r, v);
        addNormalAsByte(v);
        vertices.emplace_back(v + t.vertices[i]);
        addColorAsByte(r);
    }
}

// Project previous "sides"-sided polygon to the plane from vertex and gradient "i" in Tract "t"
void TractDataWrapper::projectContour(int sides, Tract& t, int i) {
    auto& n = t.gradient[i]; // Plane normal
    auto& p0 = t.vertices[i]; // Point on plane
    auto l = glm::normalize(t.vertices[i] - t.vertices[i - 1]); // Line direction

    // Project all vertices of the previous contour onto the plane
    for (int s = 0; s < sides; s++) {
        auto& l0 = vertices[vertices.size() - sides]; // Line starting point
        auto d = glm::dot(p0 - l0, n) / glm::dot(l, n); // Distance from point to plane along line
        auto p = l0 + l * d; // Intersection point of line and plane
        addNormalAsByte(p - t.vertices[i]);
        vertices.push_back(p);
        addColorAsByte(n);
    }
}

void TractDataWrapper::generateAverageTract(int nrOfPoints) {
    for (int i = 0; i < nrOfPoints; ++i) {
        glm::vec3 avg = {0, 0, 0};
        float count = 0;

        for (Tract t: data) {
            for (int j = i * t.vertices.size() / nrOfPoints; j < (i + 1) * t.vertices.size() / nrOfPoints; j++) {
                avg = (count * avg + t.vertices[j]) / (count + 1);
                count++;
            }
        }

        if (i > 0) {
            avgTract.gradient.push_back(avg - avgTract.vertices.back());
        }

        avgTract.vertices.push_back(avg);

        float maxDistance;
        for (Tract t: data) {
            for (int j = i * t.vertices.size() / nrOfPoints; j < (i + 1) * t.vertices.size() / nrOfPoints; j++) {
                maxDistance = glm::max(glm::distance(avg, t.vertices[j]), maxDistance);
            }
        }

        avgTractWidth.push_back(maxDistance);
        avgTract.indices.push_back(i);
    }

    avgTract.gradient.push_back(avgTract.gradient.back());
}

glm::vec3 TractDataWrapper::getBezierPosition(int t) {
    return {1, 0, 0};
}

glm::vec3 TractDataWrapper::getBezierDirection(int t) {
    return {1, 0, 0};
}

void TractDataWrapper::addColorAsByte(const glm::vec3& clr) {
    auto a = glm::abs(clr);
    colors.emplace_back(abs(a.r) * 255);
    colors.emplace_back(abs(a.g) * 255);
    colors.emplace_back(abs(a.b) * 255);
}

void TractDataWrapper::addNormalAsByte(const glm::vec3& nor) {
    auto n = glm::normalize(nor);
    normals.emplace_back(n.x * 127);
    normals.emplace_back(n.y * 127);
    normals.emplace_back(n.z * 127);
}

void TractDataWrapper::init() {
    if (!enabled) return;

    // Use primitive restart to detect beginning of new tract in buffer.
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xFFFFFFFF);

    vertices.clear();
    colors.clear();
    colors2.clear();
    normals.clear();
    indices.clear();
    tractEndIndex.clear();
    if (settings.renderer == SHADED_TUBES) {
        auto start = std::chrono::high_resolution_clock::now();
        constructTubes(settings.nrOfSides);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        Info("Constructed tubes with " << settings.nrOfSides << " sides in " << duration.count() << "ms");
    } else {
        auto start = std::chrono::high_resolution_clock::now();
        for (auto tract: data) {
            for (const auto& g: tract.gradient) {
                addColorAsByte(g);
                addNormalAsByte(g);
            }
            for (int i = 0; i < tract.vertices.size(); ++i) {
                ssboData.push_back({tract.vertices[i].x, tract.vertices[i].y, tract.vertices[i].z, tract.gradient[i].x,
                                    tract.gradient[i].y, tract.gradient[i].z});
            }
            colors2.insert(colors2.end(), tract.gradient.begin(), tract.gradient.end());
            vertices.insert(vertices.end(), tract.vertices.begin(), tract.vertices.end());
            indices.insert(indices.end(), tract.indices.begin(), tract.indices.end());
            tractEndIndex.push_back(indices.size());
            indices.push_back(0xFFFFFFFF);
        }
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        Info("Constructed lines in " << duration.count() << "ms");
    }

    // |vertices| == |colors| == |normals|
    auto vs = (long long) (vertices.size() * sizeof(glm::vec3));
    auto cs = (long long) (colors.size() * sizeof(uint8_t));
    auto ns = (long long) (normals.size() * sizeof(uint8_t));

    // Bind Vertex Array Object
    glBindVertexArray(VAO);

    // Copy Vertex data
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Allocate memory
//    glBufferData(GL_ARRAY_BUFFER, ns, nullptr, GL_STATIC_DRAW);
    // Positions
//    glBufferSubData(GL_ARRAY_BUFFER, 0, vs, &vertices[0]);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
//    glEnableVertexAttribArray(0);
//    // Colors
//    glBufferSubData(GL_ARRAY_BUFFER, vs, cs, &colors[0]);
//    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*) vs);
//    glEnableVertexAttribArray(1);
//    // Normals
//    glBufferSubData(GL_ARRAY_BUFFER, 0, ns, &normals[0]);
//    glVertexAttribPointer(2, 3, GL_BYTE, GL_TRUE, 0, (void*) 0);
//    glEnableVertexAttribArray(2);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ssboData.size() * sizeof(ssboUnit), &ssboData[0], GL_STATIC_DRAW);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

void TractDataWrapper::draw() {
    glBindVertexArray(VAO);
    settings.shader.setFloat("alpha", alpha);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, SSBO);
    if (settings.renderer == SHADED_TUBES) {
        glDrawElements(GL_TRIANGLE_STRIP, (int) tractEndIndex[showTractCount - 1], GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawElements(GL_LINE_STRIP, (int) tractEndIndex[showTractCount - 1], GL_UNSIGNED_INT, nullptr);
    }
}

TractDataWrapper::TractDataWrapper(std::string name, const std::string& filePath) : TractDataWrapper(std::move(name)) {
    parse(filePath, true);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &SSBO);
    TractDataWrapper::init();
}

TractDataWrapper::~TractDataWrapper() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &SSBO);
}


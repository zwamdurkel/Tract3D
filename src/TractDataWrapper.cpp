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

void TractDataWrapper::init() {
    if (!enabled) return;

    counts.clear();
    firsts.clear();
    auto start = std::chrono::high_resolution_clock::now();
    int firstOffset = 0;
    for (auto tract: data) {
        if (settings.renderer == SHADED_TUBES) {
            counts.insert(counts.end(), tract.vertices.size() - 1, settings.nrOfSides * 2 + 2);
            for (int i = 1; i < tract.vertices.size(); i++) {
                firsts.push_back(firstOffset);
                firstOffset += settings.nrOfSides * 2 + 2;
            }
            firstOffset += settings.nrOfSides * 2 + 2;
        } else {
            counts.emplace_back(tract.vertices.size());
            firsts.emplace_back(firstOffset);
            firstOffset += tract.vertices.size();
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    Info("Initialized draw parameters in " << duration.count() << "ms");
}

void TractDataWrapper::draw() {
    glBindVertexArray(VAO);
    settings.shader.setFloat("alpha", alpha);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, SSBO);
    if (settings.renderer == SHADED_TUBES) {
        glMultiDrawArrays(GL_TRIANGLE_STRIP, &firsts[0], &counts[0], counts.size() * showTractCount / tractCount);
    } else {
        glMultiDrawArrays(GL_LINE_STRIP, &firsts[0], &counts[0], counts.size() * showTractCount / tractCount);
        if (settings.drawPoints) {
            glMultiDrawArrays(GL_POINTS, &firsts[0], &counts[0], counts.size() * showTractCount / tractCount);
        }
    }
}

TractDataWrapper::TractDataWrapper(std::string name, const std::string& filePath) : TractDataWrapper(std::move(name)) {
    parse(filePath, true);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &SSBO);
    for (auto tract: data) {
        for (int i = 0; i < tract.vertices.size(); ++i) {
            ssboData.push_back({tract.vertices[i].x, tract.vertices[i].y, tract.vertices[i].z, tract.gradient[i].x,
                                tract.gradient[i].y, tract.gradient[i].z});
        }
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ssboData.size() * sizeof(ssboUnit), &ssboData[0], GL_STATIC_DRAW);
    glPointSize(5);

    TractDataWrapper::init();
}

TractDataWrapper::~TractDataWrapper() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &SSBO);
}


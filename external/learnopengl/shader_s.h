#pragma once

#include "../glad/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include "../../src/logger.h"

/*
 * Taken from the LearnOpenGL repository. This helper class takes care of initializing a shader program.
 * https://github.com/JoeyDeVries/LearnOpenGL
 *
 * Some modifications have been made.
 */

class Shader {
public:
    unsigned int ID{};

    Shader() = default;

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const std::string& computePath) {
        ID = glCreateProgram();
        build(computePath, GL_COMPUTE_SHADER, "COMPUTE");

        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
    }


    Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "") {
        ID = glCreateProgram();

        build(vertexPath, GL_VERTEX_SHADER, "VERTEX");
        build(fragmentPath, GL_FRAGMENT_SHADER, "FRAGMENT");
        if (!geometryPath.empty())
            build(geometryPath, GL_GEOMETRY_SHADER, "GEOMETRY");

        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        Info("Shader program created:\n\t" << vertexPath << "\n\t" << fragmentPath);
    }

    void build(const std::string& shaderPath, int shaderType, const std::string& typeString) {
        // 1. retrieve the shader/fragment source code from filePath
        std::string shaderCode;
        std::ifstream shaderFile;
        // ensure ifstream objects can throw exceptions:
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            shaderFile.open(shaderPath);
            std::stringstream shaderStream;
            // read file's buffer contents into streams
            shaderStream << shaderFile.rdbuf();
            // close file handlers
            shaderFile.close();
            // convert stream into string
            shaderCode = shaderStream.str();
        } catch (std::ifstream::failure& e) {
            Error("ERROR::" << typeString << "::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what());
        }
        const char* shaderCodeString = shaderCode.c_str();
        // 2. compile shaders
        unsigned int shader;
        // shader
        shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderCodeString, NULL);
        glCompileShader(shader);
        checkCompileErrors(shader, typeString);
        // shader Program
        glAttachShader(ID, shader);

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(shader);
    }

    // activate the shader
    // ------------------------------------------------------------------------
    void use() {
        glUseProgram(ID);
    }

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
    }

    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                Error("ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                    << "\n -- --------------------------------------------------- -- ");
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                Error("ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
                    << "\n -- --------------------------------------------------- -- ");
            }
        }
    }
};

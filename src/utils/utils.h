#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

// read text files, you might need to convert string to char 
std::string readFile(const std::string& path) {
    // open the file 
    std::ifstream file(path); 
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file at " << path << std::endl;
    }

    std::string file_lines;
    std::string line;

    // Read the file line by line
    while (std::getline(file, line)) {
        file_lines += line + "\n";
    }
    return file_lines;
}

// create and compile shaders
unsigned int compileShader(unsigned int type, std::string source) {
    source = readFile(source);
    const char* shaderSource = source.c_str();

    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        cerr << "Shader compilation failed:\n" << infoLog << endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
};

unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[1024];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        cerr << "Shader program linking failed:\n" << infoLog << endl;
        glDeleteProgram(shaderProgram);
        return 0;
    }

    return shaderProgram;
};
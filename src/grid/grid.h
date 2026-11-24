#pragma once
#include <glad/glad.h>
#include "../utils/utils.h"
using namespace std;

struct GridResources {
    unsigned int gridVAO;
    unsigned int gridShaderProgram;
};

GridResources makeGrid() {
    GridResources grid;

    glGenVertexArrays(1, &grid.gridVAO);
    glBindVertexArray(grid.gridVAO);

    // Shaders and stuff    
    unsigned int gridVertexShader;
    gridVertexShader = compileShader(GL_VERTEX_SHADER, "../src/grid/shaders/grid_shaders/grid.vert");

    unsigned int gridFragmentShader;
    gridFragmentShader = compileShader(GL_FRAGMENT_SHADER, "../src/grid/shaders/grid_shaders/grid.frag");

    grid.gridShaderProgram = createShaderProgram(gridVertexShader, gridFragmentShader);

    return grid;
}
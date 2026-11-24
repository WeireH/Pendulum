#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera/camera.h"
#include "utils/utils.h"
#include "grid/grid.h"
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

// Width and Height of the screen
int width = 800;
int height = 600;

// Current width and heigt of the app
float currentWidth = 800;
float currentHeight = 600;

// returns the width and height of the screen
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    currentWidth = width;
    currentHeight = height;
}  

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Inputs, like keyboards and etc
void processInput(GLFWwindow* window) {
    const float cameraSpeed = 10.0 * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) *
    cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) *
    cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraUp
    ;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraUp
    ;
}

    float vertices[24] = {
        // front
        -0.5f, 0.5f, 0.5f,
        0.5f,  0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,

        // back
        -0.5f, 0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f

    };

int main(){   
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    

    // to run on apple
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Creating window
    GLFWwindow* window = glfwCreateWindow(width, height, "Pendulum", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // If cant create window
    if (window == nullptr) {
        cout << "Failed to create window" << endl;
        glfwTerminate();
        return -1;
    }

    // initlaizing glad
    GLADloadproc proc = (GLADloadproc)glfwGetProcAddress;
    bool success = gladLoadGLLoader(proc);
    if (!success) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // updates the system on the size of the screen
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    int framebufferWidth;
    int framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    currentWidth = framebufferWidth;
    currentHeight = framebufferHeight;

    glViewport(0, 0, framebufferWidth, framebufferHeight);

    // Shaders and stuff    
    unsigned int vertexShader;
    vertexShader = compileShader(GL_VERTEX_SHADER, "../assets/shaders/vertex.glsl");

    if (vertexShader == 0) {
        glfwTerminate();
        return -1;
    }

    unsigned int fragmentShader;
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, "../assets/shaders/fragment.glsl");

    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glfwTerminate();
        return -1;
    }

    unsigned int shaderProgram;
    shaderProgram = createShaderProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (shaderProgram == 0) {
        glfwTerminate();
        return -1;
    }

    unsigned int indices[36] = {
        // Front face
        0, 2, 1,
        2, 3, 1,
        
        // Back face
        4, 5, 6,
        5, 7, 6,
        
        // Left face
        4, 6, 0,
        6, 2, 0,
        
        // Right face
        1, 3, 5,
        3, 7, 5,
        
        // Top face
        4, 0, 5,
        0, 1, 5,
        
        // Bottom face
        2, 6, 3,
        6, 7, 3
    };

    // VAO and VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );   

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(
        0,                  // location = 0 of vertex shader
        3,                  // x, y, z
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);  
    glBindVertexArray(0);

    // depth
    glEnable(GL_DEPTH_TEST);

    // Enable wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPointSize(10.0f);

    // grid shader
    GridResources grid = makeGrid();

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window)){
        // used to control the velocity of the camera
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // inputs
        processInput(window);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        // rendering stuff 
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

         // Model and Projection Matrices
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        model = glm::translate(
            model,
            glm::vec3(0.0f, 0.0f, 0.0f)
        );

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraFront = glm::normalize(direction);

        view = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp
        );

        // Make perspective with projection
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        // send each matrix to the vertex shader
        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "model"),
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "view"),
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "projection"),
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,
                36, 
                GL_UNSIGNED_INT, 
                0
            );
        
        // Grid Shaders
        glUseProgram(grid.gridShaderProgram);

        // send each matrix to the vertex shader
        glUniformMatrix4fv(
            glGetUniformLocation(grid.gridShaderProgram, "model"),
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(grid.gridShaderProgram, "view"),
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(grid.gridShaderProgram, "projection"),
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        // get locations
        GLint resolutionLocation = glGetUniformLocation(grid.gridShaderProgram, "u_resolution");
        glm::vec2 u_resolution(currentWidth, currentHeight);

        GLint cameraPosLoc = glGetUniformLocation(grid.gridShaderProgram, "cameraPosShader");
        glm::vec3 cameraPosShader(cameraPos);

        // send values
        glUniform2fv(resolutionLocation, 1, glm::value_ptr(u_resolution));

        glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPosShader));


        glBindVertexArray(grid.gridVAO);
        glDepthMask(GL_FALSE);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDepthMask(GL_TRUE);


        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();  
    };
    
    glfwTerminate();
    return 0;
};

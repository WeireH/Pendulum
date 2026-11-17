#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera/camera.h"
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

// Width and Height of the screen
int width;
int height;

// Current width and heigt of the app
float currentWidth;
float currentHeight;

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Pendulum", nullptr, nullptr);
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPointSize(10.0f);

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
        glClearColor(0.18f, 0.01f, 0.02f, 1.0f);
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

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

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

            glDrawElements(GL_POINTS,
                36, 
                GL_UNSIGNED_INT, 
                0
            );
        
        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();  
    };
    
    glfwTerminate();
    return 0;
}
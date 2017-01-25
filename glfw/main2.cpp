//
//  main.cpp
//  glfw
//
//  Created by tuyoo on 16/8/4.
//  Copyright © 2016年 tuyoo. All rights reserved.
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "SOIL.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <map>

using namespace std;

char* readFile(const char *name)
{
    FILE* fp = fopen(name, "r");
    char *shaderSource;
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    shaderSource = new char[len+1];
    rewind(fp);
    fread(shaderSource, 1, len, fp);
    shaderSource[len] = 0;
    fclose(fp);
    return shaderSource;
}

GLuint loadShader(const char *name, unsigned int type)
{
    char *shaderSource = readFile(name);
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    delete[] shaderSource;
    glCompileShader(shader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << name << "########" << infoLog << std::endl;
        exit(1);
    }
    return shader;
}

GLuint loadTexture(const char *name, int &width, int &height, bool alpha = true)
{
    unsigned char* image = SOIL_load_image(name, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    
    //Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );	// Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;

}

GLuint getShaderProgram(const char *name)
{
    GLuint shaderProgram = glCreateProgram();
    char vertPath[100];
    sprintf(vertPath, "%s.vert", name);
    GLuint vertexShader = loadShader(vertPath, GL_VERTEX_SHADER);
    char fragPath[100];
    sprintf(fragPath, "%s.frag", name);
    GLuint fragmentShader = loadShader(fragPath, GL_FRAGMENT_SHADER);
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
        exit(1);
    }
    return shaderProgram;
}

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
bool keys[1024];
GLfloat deltaTime = 0.0f;   // 当前帧遇上一帧的时间差
GLfloat lastFrame = 0.0f;   // 上一帧的时间
GLfloat lastX = 400, lastY = 300;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

bool firstMouse = true;
GLfloat yaw   = -90.0f;
GLfloat pitch =   0.0f;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    GLfloat sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw   += xoffset;
    pitch += yoffset;
    
    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

GLfloat fov =  45.0f;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

void do_movement()
{
    // 摄像机控制
    GLfloat cameraSpeed = 2.0f * deltaTime;
    if(keys[GLFW_KEY_W])
        cameraPos += cameraSpeed * cameraFront;
    if(keys[GLFW_KEY_S])
        cameraPos -= cameraSpeed * cameraFront;
    if(keys[GLFW_KEY_A])
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(keys[GLFW_KEY_D])
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main(int argc, const char * argv[]) {
    int width, height;
    
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 设置清空颜色
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    
    //glEnable(GL_CULL_FACE);

    // 按键检测
    glfwSetKeyCallback(window, key_callback);
    // 鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    // 滚轮
    glfwSetScrollCallback(window, scroll_callback);
    

//    GLuint texture1 = loadTexture("container2.png", width, height);
//    GLuint texture2 = loadTexture("container2_specular.png", width, height);
//    GLuint texture3 = loadTexture("matrix.jpg", width, height);
    
    // Load textures
    GLuint cubeTexture = loadTexture("container2.png", width, height, false);
    GLuint texture2 = loadTexture("container2_specular.png", width, height, false);
    GLuint floorTexture = loadTexture("container2.png", width, height, false);
    GLuint transparentTexture = loadTexture("window.png", width, height);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, transparentTexture);
    glActiveTexture(GL_TEXTURE0);
    
    // Positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    
    // 三角形着色器
    GLuint triangleShaderProgram = getShaderProgram("triangle");
    glUseProgram(triangleShaderProgram);
    
    glUniform1i(glGetUniformLocation(triangleShaderProgram, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(triangleShaderProgram, "material.specular"), 1);
    //glUniform1i(glGetUniformLocation(triangleShaderProgram, "material.emission"), 2);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "material.shininess"), 32.0f);
    
    // Directional light
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "dirLight.direction"), 0.0f, 0.0f, -0.3f);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
    // Point light 1
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[0].linear"), 0.09);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[0].quadratic"), 0.032);
    // Point light 2
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[1].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[1].linear"), 0.09);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[1].quadratic"), 0.032);
    // Point light 3
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[2].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[2].linear"), 0.09);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[2].quadratic"), 0.032);
    // Point light 4
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(triangleShaderProgram, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[3].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[3].linear"), 0.09);
    glUniform1f(glGetUniformLocation(triangleShaderProgram, "pointLights[3].quadratic"), 0.032);
    
    
    GLint lightDirPos = glGetUniformLocation(triangleShaderProgram, "light.direction");
    GLint lightAmbientLoc = glGetUniformLocation(triangleShaderProgram, "light.ambient");
    GLint lightDiffuseLoc = glGetUniformLocation(triangleShaderProgram, "light.diffuse");
    GLint lightSpecularLoc = glGetUniformLocation(triangleShaderProgram, "light.specular");
    
    glUniform3f(lightDirPos, -0.2f, -1.0f, -0.3f);
    glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
    glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);// 让我们把这个光调暗一点，这样会看起来更自然
    glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
  
    
    GLfloat cubeVertices[] = {
        // Positions           // Normals           // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    GLfloat planeVertices[] = {
        // Positions          // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
        5.0f, -0.5f,  5.0f, 0.0f,  0.0f, 1.0f, 2.0f,  0.0f,
        -5.0f, -0.5f,  5.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f,  0.0f, 1.0f, 0.0f,  2.0f,
        
        5.0f, -0.5f,  5.0f, 0.0f,  0.0f, 1.0f, 2.0f,  0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f,  0.0f, 1.0f, 0.0f,  2.0f,
        5.0f, -0.5f, -5.0f, 0.0f,  0.0f, 1.0f, 2.0f,  2.0f
    };
    GLfloat transparentVertices[] = {
        // Positions         // Texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f,
        0.0f, -0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f,
        1.0f, -0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 1.0f,  1.0f,
        
        0.0f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f,
        1.0f, -0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 1.0f,  1.0f,
        1.0f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 1.0f,  0.0f
    };
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);


//    GLuint vao;
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//    
//    glUseProgram(triangleShaderProgram);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
//    glEnableVertexAttribArray(2);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
    
    // Setup cube VAO
    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
    glBindVertexArray(0);
    // Setup plane VAO
    GLuint planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
    // Setup transparent plane VAO
    GLuint transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
    glBindVertexArray(0);
    
//    // 着色器
//    GLuint sampleShaderProgram = getShaderProgram("sample");
//    glUseProgram(sampleShaderProgram);
//    glUniform1i(glGetUniformLocation(triangleShaderProgram, "ourTexture"), 0);
    
    // 光着色器
    GLuint lightShaderProgram = getShaderProgram("light");
    glUseProgram(lightShaderProgram);
    
    // 灯
    GLuint lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    std::vector<glm::vec3> windows;
    windows.push_back(glm::vec3(-1.5f,  0.0f, -0.48f));
    windows.push_back(glm::vec3( 1.5f,  0.0f,  0.51f));
    windows.push_back(glm::vec3( 0.0f,  0.0f,  0.7f));
    windows.push_back(glm::vec3(-0.3f,  0.0f, -2.3f));
    windows.push_back(glm::vec3( 0.5f,  0.0f, -0.6f));

    while(!glfwWindowShouldClose(window))
    {
        
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // 检查事件
        glfwPollEvents();
        do_movement();
        // 渲染
        // 清空颜色缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Sort windows
        std::map<GLfloat, glm::vec3> sorted;
        for (GLuint i = 0; i < windows.size(); i++)
        {
            GLfloat distance = glm::length(cameraPos - windows[i]);
            sorted[distance] = windows[i];
        }
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov), (GLfloat)800 / (GLfloat)600, 0.1f, 100.0f);
        GLint modelLoc;
        GLint viewLoc;
        GLint projLoc;
        GLint viewPosLoc;
        
        // 记得激活着色器
        glUseProgram(triangleShaderProgram);
        modelLoc = glGetUniformLocation(triangleShaderProgram, "model");
        viewLoc = glGetUniformLocation(triangleShaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        projLoc = glGetUniformLocation(triangleShaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        viewPosLoc = glGetUniformLocation(triangleShaderProgram, "viewPos");
        glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

        // SpotLight
        glUniform3f(glGetUniformLocation(triangleShaderProgram, "spotLight.position"), cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(glGetUniformLocation(triangleShaderProgram, "spotLight.direction"), cameraFront.x, cameraFront.y, cameraFront.z);
        glUniform3f(glGetUniformLocation(triangleShaderProgram, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(triangleShaderProgram, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(triangleShaderProgram, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(triangleShaderProgram, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(triangleShaderProgram, "spotLight.linear"), 0.09);
        glUniform1f(glGetUniformLocation(triangleShaderProgram, "spotLight.quadratic"), 0.032);
        glUniform1f(glGetUniformLocation(triangleShaderProgram, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(triangleShaderProgram, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));
        
//        glm::vec3 lightColor(1.0);
////        lightColor.x = sin(glfwGetTime() * 2.0f);
////        lightColor.y = sin(glfwGetTime() * 0.7f);
////        lightColor.z = sin(glfwGetTime() * 1.3f);
//        
//        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
//        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
//        
//        glUniform3f(lightAmbientLoc, ambientColor.x, ambientColor.y, ambientColor.z);
//        glUniform3f(lightDiffuseLoc, diffuseColor.x, diffuseColor.y, diffuseColor.z);

//        for(int i = 0; i < 1; i++)
//        {
//            glm::mat4 model;
//            model = glm::translate(model, cubePositions[i]);
//            model = glm::rotate(model, glm::radians((GLfloat)glfwGetTime() * 50.0f), glm::vec3(i * 0.1f, 1.0f, 0.0f));
//            
//            
//            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//            
//            // 绘制三角形
//            glBindVertexArray(vao);
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//            glBindVertexArray(0);
//        }
//
//        glUseProgram(lightShaderProgram);
//
//        modelLoc = glGetUniformLocation(lightShaderProgram, "model");
//        viewLoc = glGetUniformLocation(lightShaderProgram, "view");
//        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//        projLoc = glGetUniformLocation(lightShaderProgram, "projection");
//        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
//        glBindVertexArray(lightVAO);
//        for (GLuint i = 0; i < 4; i++)
//        {
//            glm::mat4 model = glm::mat4();
//            model = glm::translate(model, pointLightPositions[i]);
//            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
//            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//        }
//        glBindVertexArray(0);
        glUniform1i(glGetUniformLocation(triangleShaderProgram, "material.specular"), 1);
        glm::mat4 model;
        glUniformMatrix4fv(glGetUniformLocation(triangleShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(triangleShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // Cubes
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);  // We omit the glActiveTexture part since TEXTURE0 is already the default active texture unit. (a single sampler used in fragment is set to 0 as well by default)
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        glUniformMatrix4fv(glGetUniformLocation(triangleShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(triangleShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // Floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        model = glm::mat4();
        glUniformMatrix4fv(glGetUniformLocation(triangleShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Render windows (from furthest to nearest)
//        glUseProgram(sampleShaderProgram);
//        modelLoc = glGetUniformLocation(sampleShaderProgram, "model");
//        viewLoc = glGetUniformLocation(sampleShaderProgram, "view");
//        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//        projLoc = glGetUniformLocation(sampleShaderProgram, "projection");
//        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        glUniform1i(glGetUniformLocation(triangleShaderProgram, "material.specular"), 0);
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4();
            model = glm::translate(model, it->second);
            glUniformMatrix4fv(glGetUniformLocation(triangleShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
        // 交换缓冲
        glfwSwapBuffers(window);
    }
//    glDeleteVertexArrays(1, &vao);
//    glDeleteBuffers(1, &vbo);
    
    glfwTerminate();
    return 0;
}

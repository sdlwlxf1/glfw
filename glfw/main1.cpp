////
////  main.cpp
////  glfw
////
////  Created by tuyoo on 16/8/4.
////  Copyright © 2016年 tuyoo. All rights reserved.
////
//
//#define GLEW_STATIC
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include <stdio.h>
//#include <math.h>
//#include "SOIL.h"
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//char* readFile(const char *name)
//{
//    FILE* fp = fopen(name, "r");
//    char *shaderSource;
//    fseek(fp, 0, SEEK_END);
//    long len = ftell(fp);
//    shaderSource = new char[len+1];
//    rewind(fp);
//    fread(shaderSource, 1, len, fp);
//    shaderSource[len] = 0;
//    fclose(fp);
//    return shaderSource;
//}
//
//GLuint loadShader(const char *name, unsigned int type)
//{
//    char *shaderSource = readFile(name);
//    GLuint shader = glCreateShader(type);
//    glShaderSource(shader, 1, &shaderSource, NULL);
//    delete[] shaderSource;
//    glCompileShader(shader);
//    GLint success;
//    GLchar infoLog[512];
//    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//    if(!success)
//    {
//        glGetShaderInfoLog(shader, 512, NULL, infoLog);
//        std::cout << infoLog << std::endl;
//        exit(1);
//    }
//    return shader;
//}
//
//
//
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
//{
//    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, GL_TRUE);
//}
//
//int main(int argc, const char * argv[]) {
//    int width, height;
//    
//    
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
//    if(window == nullptr)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//    glewExperimental = GL_TRUE;
//    if(glewInit() != GLEW_OK)
//    {
//        std::cout << "Failed to initialize GLEW" << std::endl;
//        return -1;
//    }
//    glfwGetFramebufferSize(window, &width, &height);
//    glViewport(0, 0, width, height);
//    
//    // 设置清空颜色
//    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//    
//    //glEnable(GL_CULL_FACE);
//
//    // 按键检测
//    glfwSetKeyCallback(window, key_callback);
//    
//    
//    
//    GLuint vao;
//    GLuint vbo;
//    GLuint ebo;
//    GLuint shaderProgram;
//    
//    
//    shaderProgram = glCreateProgram();
//    GLuint vertexShader = loadShader("triangle.vert", GL_VERTEX_SHADER);
//    GLuint fragmentShader = loadShader("triangle.frag", GL_FRAGMENT_SHADER);
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glLinkProgram(shaderProgram);
//    glDeleteShader(vertexShader);
//    glDeleteShader(fragmentShader);
//    GLint success;
//    GLchar infoLog[512];
//    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//    if(!success) {
//        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//        std::cout << infoLog << std::endl;
//        exit(1);
//    }
//    
//    glUseProgram(shaderProgram);
//    
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//    
//    glGenBuffers(1, &vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    GLfloat vertices[] = {
//        //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
//        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
//        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
//        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
//        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
//    };
//    GLuint indices[] = {  // Note that we start from 0!
//        0, 1, 3,  // First Triangle
//        1, 2, 3   // Second Triangle
//    };
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    
//    glGenBuffers(1, &ebo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//    
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
//    glEnableVertexAttribArray(2);
//    
//    
//    
//    unsigned char* image = SOIL_load_image("container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
//    
//    GLuint texture1;
//    glGenTextures(1, &texture1);
//    
//    glBindTexture(GL_TEXTURE_2D, texture1);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
//    glGenerateMipmap(GL_TEXTURE_2D);
//    
//    SOIL_free_image_data(image);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    
//    image = SOIL_load_image("awesomeface.png", &width, &height, 0, SOIL_LOAD_RGB);
//    
//    GLuint texture2;
//    glGenTextures(1, &texture2);
//    
//    glBindTexture(GL_TEXTURE_2D, texture2);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
//    glGenerateMipmap(GL_TEXTURE_2D);
//    
//    SOIL_free_image_data(image);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    glBindVertexArray(0);
//    
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, texture1);
//    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture1"), 0);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, texture2);
//    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture2"), 1);
//    
//    while(!glfwWindowShouldClose(window))
//    {
//        // 检查事件
//        glfwPollEvents();
//        
//        // 渲染
//        // 清空颜色缓冲
//        glClear(GL_COLOR_BUFFER_BIT);
//        
//        // 记得激活着色器
//        glUseProgram(shaderProgram);
//
//        glm::mat4 trans;
//        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
//        trans = glm::rotate(trans, glm::radians((GLfloat)glfwGetTime() * 50.0f), glm::vec3(0.0f,0.0f,1.0f));
//        
//        GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
//        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
//        // 绘制三角形
//        glBindVertexArray(vao);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//        //glDrawArrays(GL_TRIANGLES, 0, 3);
//        glBindVertexArray(0);
//        
//        
//        trans = glm::mat4(1.0f);
//        trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
//        trans = glm::scale(trans, glm::vec3(glm::sin((GLfloat)glfwGetTime()),glm::sin((GLfloat)glfwGetTime()),glm::sin((GLfloat)glfwGetTime())));
//        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
//        // 绘制三角形
//        glBindVertexArray(vao);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//        //glDrawArrays(GL_TRIANGLES, 0, 3);
//        glBindVertexArray(0);
//        
//        // 交换缓冲
//        glfwSwapBuffers(window);
//    }
//    glDeleteVertexArrays(1, &vao);
//    glDeleteBuffers(1, &vbo);
//    
//    glfwTerminate();
//    return 0;
//}

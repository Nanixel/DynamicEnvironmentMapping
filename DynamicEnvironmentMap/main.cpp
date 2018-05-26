//
//  main.cpp
//  DynamicEnvironmentMap
//
//  Created by Jesse Tellez on 5/24/18.
//  Copyright © 2018 Jesse. All rights reserved.
//

#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"

#include <iostream>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
GLFWwindow* SetupGLFW();
void configureOpenGL();
unsigned int loadTexture(const char *path, bool isTransparent);

void createSphere();
unsigned int CreateSkybox();
unsigned int CreateSkyboxTexture();
unsigned int CreateFrameBuffer(unsigned int texture);
unsigned int createEmptyCubemap();
unsigned int test();

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int AMOUNT = 10000;

unsigned int sphereVAO = 0;
unsigned int indexCount;

unsigned int dynamicCubeTex;

const int size = 2048;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main(int argc, const char * argv[]) {
    
    GLFWwindow* window = SetupGLFW();
    if (window == nullptr) {
        return -1;
    }
    
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    configureOpenGL();
    
    Shader skyboxShader("/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/shaders/SkyboxVertex.glsl", "/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/shaders/SkyboxFragment.glsl");
    
    Shader shader("/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/shaders/Vertex.glsl", "/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/shaders/Fragment.glsl");
    
    Shader base("/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/shaders/BaseVertex.glsl", "/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/shaders/BaseFragment.glsl");
   
    //test();
    
    unsigned int skybox = CreateSkybox();
    unsigned int skyboxTexure = CreateSkyboxTexture();
    unsigned int marbleTexture = loadTexture("/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/resources/marble.jpg", false);
    
    //unsigned int emptyCubemap = createEmptyCubemap();
    
    createSphere();
    
    //unsigned int framebuffer = CreateFrameBuffer(emptyCubemap);
    unsigned int framebuffer = test();
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    
    shader.use();
    shader.setInt("skybox", 0);
    
    base.use();
    base.setInt("marble", 0);
    
    //these are front vectors
    std::vector<glm::vec3> targetVectors = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
        
    };
    
    std::vector<glm::vec3> upVectors = {
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f)
    };
    
    glm::vec3 modelOrigin(0.0f, 0.0f, 0.0f);
    
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        float fov = 2.0 * glm::atan(size / (size - 0.5));
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 100.0f);
        glm::mat4 model;
        
        glViewport(0, 0, size, size);
        
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        
           // glClearColor(0.41f, 0.33f, 0.32f, 1.0f);
       
        
            for (unsigned int i = 0; i < 6; i++) {
                // right
                // left
                // top
                // bottom
                // back
                // front
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dynamicCubeTex, 0);
                glClear(GL_DEPTH_BUFFER_BIT);
                // ----------------- SPHERE 1 -------------------
                glm::mat4 currentCubemapView;
                currentCubemapView = glm::lookAt(modelOrigin, modelOrigin + targetVectors[i], upVectors[i]);
                
                base.use();
                glBindVertexArray(sphereVAO);
                base.setMat4("view",  currentCubemapView);
                base.setMat4("projection", projection);
                float x = 3.0f * glm::sin(currentFrame);
                float z = 3.0f * glm::cos(currentFrame);
                
                //model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0, 1.0f, 0.0));
                model = glm::mat4(glm::translate(model, glm::vec3(x, 0.0f, z)));
                model = glm::scale(model, glm::vec3(1.0f));
                // y is up and down here
                //model = glm::mat4(glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f)));
                
                //model = glm::rotate(model, 90.0f, glm::vec3(0.0, 1.0f, 0.0));
                
                base.setMat4("model", model);
                glBindTexture(GL_TEXTURE_2D, marbleTexture);
                glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                
                // ----------------- SKYBOX --------------------
                
                skyboxShader.use();
                skyboxShader.setMat4("view", glm::mat4(glm::mat3(currentCubemapView)));
                skyboxShader.setMat4("projection", projection);

               // glDepthMask(GL_FALSE);
                glDepthFunc(GL_LEQUAL);
                glBindVertexArray(skybox);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexure);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
               // glDepthMask(GL_TRUE);
                glDepthFunc(GL_LESS);
            }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        model = glm::mat4();
        
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.41f, 0.33f, 0.32f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        //--------------- SPHERE --------------------
        
        glBindVertexArray(sphereVAO);

        shader.use();
        shader.setMat4("view", camera.GetViewMatrix());
        shader.setMat4("projection", projection);

        model = glm::mat4();
        shader.setMat4("model", model);
        // Given the position of my camera...sampler a color from the generated cube map texture
        shader.setVec3("cameraPos", camera.Position);
        glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeTex);

        glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);

        base.use();
        base.setMat4("view", camera.GetViewMatrix());
        base.setMat4("projection", projection);
        float x = 3.0f * glm::sin(currentFrame);
        float z = 3.0f * glm::cos(currentFrame);
        model = glm::mat4(glm::translate(model, glm::vec3(x, 0.0f, z)));
        //model = glm::mat4(glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f)));
        model = glm::scale(model, glm::vec3(0.5f));
        base.setMat4("model", model);
        glBindTexture(GL_TEXTURE_2D, marbleTexture);
        glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
        
        skyboxShader.use();
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(skybox);
        
        //front and back are the smallest, left and right are the largest
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexure);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        
        glBindVertexArray(0);
        
        processInput(window);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

void configureOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

GLFWwindow* SetupGLFW() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        //return -1;
    } else {
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        
        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    return window;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    float speed = 2.5f;
    float velocity = speed * deltaTime;
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    // 0, 0 is bottom left corner
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const *path, bool isTransparent)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        if (isTransparent) {
            // If a texture is transparent, than we want to set wraping to clamping, otherwise the texture will repeat and you will see the bottom border at the top of each texture.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}


unsigned int CreateSkybox() {
    
    float skybox[] = {
        // Positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox), skybox, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    
    glBindVertexArray(0);
    
    return skyboxVAO;
}

unsigned int CreateSkyboxTexture() {
    std::vector<std::string> faces {
        "/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/resources/right.jpg",
        "/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/resources/left.jpg",
        "/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/resources/top.jpg",
        "/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/resources/bottom.jpg",
        "/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/resources/front.jpg",
        "/Users/jessetellez/Desktop/DynamicEnvironmentMap/DynamicEnvironmentMap/resources/back.jpg",
    };
    
    int width, height, nrChannels;
    unsigned int cubemap;
    unsigned char* data;
    
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    
    for (unsigned int face = 0; face < faces.size(); face++) {
        data = stbi_load(faces[face].c_str(), &width, &height, &nrChannels, 0);
        
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "ERROR LOADING CUBEMAP" << std::endl;
        }
        
        stbi_image_free(data);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    return cubemap;
}


void createSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);
        
        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;
        
        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                
                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }
        
        bool oddRow = false;
        for (int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();
        
        std::vector<float> data;
        for (int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 2 + 3) * sizeof(float);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    }
    
    glBindVertexArray(0);
}

unsigned int CreateFrameBuffer(unsigned int texture) {
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    
    unsigned int framebuffer;
    unsigned int rbo;
    
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture, 0);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture, 0);
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "AN ERROR OCCURED WHEN CREATING THE FRAME BUFFER" << std::endl;
    }
    
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    return framebuffer;
    
}

unsigned int createEmptyCubemap() {
    unsigned int cubemap;
    
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    
    for (unsigned int face = 0; face < 6; face++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    return cubemap;
}

unsigned int test() {
    
    unsigned int depthRenderbuffer, framebuffer;
    //const int size = 1024;
    // Create empty cubemap
    glGenTextures(1, &dynamicCubeTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeTex);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    // Allocate space for each side of the cube map
    for (GLuint i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, size,
                     size, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }
    
    // Create framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    // Attach one of the faces of the cubemap texture to current framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X, dynamicCubeTex, 0);
    // Attach depth buffer to framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    // Attach only the +X cubemap texture (for completeness)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, dynamicCubeTex, 0);
    
    // Check if current configuration of framebuffer is correct
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    
    // Set default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return framebuffer;

}

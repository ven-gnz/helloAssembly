

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"
#include "stb/stb_image.h"
#include <vector>


#define MINIAUDIO_IMPLEMENTATION
#include<miniaudio/miniaudio.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

unsigned int loadTexture(const char* path);

// settings : dev conf for SD resolution, release for full hd.
const unsigned int SCR_WIDTH = 854;
const unsigned int SCR_HEIGHT = 480;
float aspect = 16 / 9;


//World data

float planeVertices[] = {
    // positions          // texture Coords
     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
     5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};

float cubeVertices[] = {
    // positions          // texture Coords
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};


// Audio setups
const std::string AUDIO_FILE_PATH = "resources/song-150bpm.wav";
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == nullptr) {
        return;
    }
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, nullptr);
    (void)pInput;
}

//camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
Camera camera(cameraPos, cameraUp);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame



std::vector<glm::vec3> cubePositions = {
    glm::vec3(-1.0, 0.0, -2.0),
    glm::vec3(2.0,0.0,-1.0),
    glm::vec3(1.0f, 0.0f, -1.0f),
    glm::vec3(-2.0f, 0.0f, -4.0f),
    glm::vec3(-3.0f, 0.0f, -3.0f)
};

int main()
{
    //GLFW consts
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //miniaudio consts
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    // dev conf
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DebugBuild", NULL, NULL);
    //release conf
    // GLFWwindow* window = glfwCreateWindow(1920, 1080, "HelloAssembly", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader simpleShader("resources/shader.vs", "resources/shader.fs");
    Shader endCubeShader("resources/shader.vs", "resources/endcube.fs");

    unsigned int marble = loadTexture("resources/marble.jpg");
    unsigned int face = loadTexture("resources/awesomeface.png");
    unsigned int container = loadTexture("resources/container.jpg");


    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
 
    glEnableVertexAttribArray(0);
   
    simpleShader.use();
    simpleShader.setInt("texture1", 0);

    endCubeShader.use();
    endCubeShader.setInt("texture2", container);
    endCubeShader.setInt("texture3", face);

    //decode audio file

    result = ma_decoder_init_file(AUDIO_FILE_PATH.c_str(), nullptr, &decoder);
    if (result != MA_SUCCESS) {
        std::cerr << "Could not load file: " << AUDIO_FILE_PATH << std::endl;
        return -2;
    }

    // playback device configuration
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &decoder;


    // initialize playback device
    if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
        std::cerr << "Failed to open playback device." << std::endl;
        ma_decoder_uninit(&decoder);
        return -3;
    }

    // Start audio playback
    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cerr << "Failed to start playback device." << std::endl;
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -4;
    }

    float lastFrame = 0.0f;
    float timePassed = 0;

    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(window))
    {
       
        processInput(window);
        float currentFrame = glfwGetTime();
        timePassed = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

        if ( timePassed < 52 ) {
            camera.Position = glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z - 0.5 * deltaTime);

            // do some lighting

            // Draw plane
            simpleShader.use();
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.1f, 100.0f);
            simpleShader.setMat4("view", view);
            simpleShader.setMat4("projection", projection);

            glBindVertexArray(planeVAO);
            glBindTexture(GL_TEXTURE_2D, marble);
            simpleShader.setMat4("model", glm::mat4(1.0f));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);


            for (unsigned int i = 0; i < sizeof(cubePositions)/sizeof(glm::vec3); i++) {
                glBindVertexArray(cubeVAO);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, marble);
                model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                simpleShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            glBindVertexArray(0);

            
        }
        // scene ending render
        else {
            if (timePassed < 52) {
                
                
                glm::mat4 view = camera.GetViewMatrix();
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.1f, 100.0f);
           
                glm::mat4 model = glm::mat4(1.0f);
                

                model = glm::translate(model, glm::vec3(0.5 * cos(timePassed), 0.5 * sin(timePassed), 0.0));
              
                endCubeShader.use();
                endCubeShader.setMat4("model", model);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, container);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, face);
                glBindVertexArray(cubeVAO);
               
                
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

           

        }
        


        //check if enough time has passed, then we render big container cube
        //Big container cube disco, if time
      
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

   
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVAO);
 

    // audio termination
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);

   
    glfwTerminate();
    return 0;
}

unsigned int loadTexture(char const* path) {

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {

        GLenum format = GL_RGBA;
        if (nrComponents == 1) format = GL_RED;
        if (nrComponents == 3) format = GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);

    }
    else {
        std::cout << "Texture loading failed : path" << path << ::std::endl;
        stbi_image_free(data);

    }
    return textureID;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


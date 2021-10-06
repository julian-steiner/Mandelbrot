#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <glm.hpp>
#include <gtx/string_cast.hpp>
#include <gtc/matrix_transform.hpp>

// Macro to raise a debugbreak on assert failures
#define ASSERT(x) if (!(x)) __debugbreak();

// Macro to call function and get info to GLLogCall
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

// Function to get rid of every error that is currently stored
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

// Function to print the first error that pops up
bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGLError] (" << error << "): " << function <<
        " " << file << ":" << line << "\n";
        return false;
    }
    return true;
}

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    // Shadertype enum and numbers coherent to position in ss array
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    // Variables for shader reading
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    
    // Go through every line
    while (getline(stream, line))
    {
        // Checking for the shader # on lines
        if (line.find("#shader") != std::string::npos)
        {
            // Set the mode to the attribute after
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }

        else
        {
            // Append lines to the position of the current mode
            ss[(int)type] << line << "\n";
        }
    }

    return {ss[(int)ShaderType::VERTEX].str(), ss[(int)ShaderType::FRAGMENT].str()};
}

unsigned int CompileShader(const std::string& sourceCode, unsigned int type)
{
    // creating the shader
    GLCall(unsigned int id = glCreateShader(type));

    // writing the program onto the shader
    const char* src = sourceCode.c_str();
    GLCall(glShaderSource(id, 1, &src, NULL));

    // Compiling the shader
    GLCall(glCompileShader(id));

    // Getting the compilation result
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE)
    {
        // Get the length of the shader error log
        int length;
        GLCall(glGetShaderiv(id,GL_INFO_LOG_LENGTH, &length));

        // Get the error log
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));

        // Pring the error to the console
        std::cout << "FAILED TO COMPILE " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cout << message << std::endl;

        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

unsigned int createShader(const std::string& vertexSource, const std::string& fragmentSource)
{
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(vertexSource, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    return program;
}

int main()
{
    glewExperimental = GL_TRUE;

    glfwInit();

    //GLFWwindow* window = glfwCreateWindow(1280, 720, "Mandelbrot", NULL, NULL);
    GLFWwindow* window = glfwCreateWindow(1080, 1080, "Mandelbrot", NULL, NULL);

    glfwMakeContextCurrent(window);

    glewInit();

    float vertexBufferData[] = 
    {
        -1.0f,     -1.0f,
        1.0f,      -1.0f,
        1.0f,      1.0f,
        -1.0f,     1.0f
    };

    unsigned int indexBufferData[] = 
    {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int vertexBuffer;
    glCreateBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GL_FLOAT), (const void*)vertexBufferData, GL_STATIC_DRAW);

    unsigned int indexBuffer;
    glCreateBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 2 * sizeof(unsigned int), (const void*)indexBufferData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, true, 2 * sizeof(float), 0);


    ShaderProgramSource shaderSource = ParseShader("../shaders/Mandelbrot.shader");
    unsigned int shaderProgram = createShader(shaderSource.VertexSource, shaderSource.FragmentSource);

    glUseProgram(shaderProgram);

    int location = glGetUniformLocation(shaderProgram, "u_Zoom");
    int zoomLocation = glGetUniformLocation(shaderProgram, "u_ZoomPosition");

    //glm::mat4 proj = glm::ortho();
    //int location = glGetUniformLocation(shaderProgram, "u_Projection");
    //glUniformMatrix4fv(location, 1, GL_FALSE, &proj[0][0]);

    float zoom = 1;
    //float increment = 1;
    float increment = 1.01;
    //glUniform2f(zoomLocation, -0.7463, 0.1102);
    //glUniform2f(zoomLocation, -1.25066, 0.02012);
    glUniform2f(zoomLocation, 0.2929859127507, 0.6117848324958);
    //glUniform2f(zoomLocation, 0.3369844464873, 0.0487782196791);
    //float increment = 1;

    while (!glfwWindowShouldClose(window))
    {
        //glClear(GL_COLOR_BUFFER_BIT);

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniform1f(location, zoom *= increment);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

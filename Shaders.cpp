#include <glad/glad.h>
#include "Shaders.h"
#include <iostream>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec4 aColor;\n"
"out vec4 ourColor;\n"
"void main() {\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec4 ourColor;\n"
"void main() { FragColor = ourColor; }\0";

unsigned int compilarShaders() {
    unsigned int vS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vS, 1, &vertexShaderSource, NULL);
    glCompileShader(vS);

    unsigned int fS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fS, 1, &fragmentShaderSource, NULL);
    glCompileShader(fS);

    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vS);
    glAttachShader(prog, fS);
    glLinkProgram(prog);

    glDeleteShader(vS);
    glDeleteShader(fS);

    return prog;
}
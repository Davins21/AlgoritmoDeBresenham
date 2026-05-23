#ifndef SHADERS_HPP
#define SHADERS_HPP

// Declaración de los códigos fuente de los shaders
extern const char* vertexShaderSource;
extern const char* fragmentShaderSource;

// Compila los shaders individuales y crea el programa final en la GPU
unsigned int compilarShaders();

#endif
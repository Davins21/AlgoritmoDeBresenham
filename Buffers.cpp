#include "VariablesGlobales.h"
#include "Buffers.h"

void inicializarBuffersGPU() {
    //Generar los identificadores únicos
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //Activar el VAO (contenedor principal)
    glBindVertexArray(VAO);

    //Llenar el VBO con las posiciones y colores de los vértices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, gridVerts.size() * sizeof(VertexSquare), gridVerts.data(), GL_DYNAMIC_DRAW);

    //Llenar el EBO con el orden en el que se conectan los triángulos
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridIndices.size() * sizeof(GLuint), gridIndices.data(), GL_STATIC_DRAW);

    // Explicarle a OpenGL cómo leer la estructura VertexSquare en memoria
    // Atributo 0: Posición (3 GLfloat)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSquare), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo 1: Color (4 GLfloat que empiezan después de la posición)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexSquare), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
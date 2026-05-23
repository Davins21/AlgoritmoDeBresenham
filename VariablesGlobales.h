#ifndef VARIABLESGLOBALES_HPP
#define VARIABLESGLOBALES_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

// Estructuras
struct VertexSquare {
    GLfloat pos[3];
    GLfloat color[4];
};

struct Punto {
    int x, y;
};

enum Herramienta { LINEA, CIRCULO, TRIANGULO, RELLENO };

// Constantes y configuraciones
const int GRID_SIZE = 150;
const int PANEL_ALTO_PX = 70;

// Declaración de variables globales (extern) para que no se dupliquen
extern std::vector<VertexSquare> gridVerts;
extern std::vector<GLuint> gridIndices;
extern unsigned int VAO, VBO, EBO;

extern int xi, yi, cxi, cyi;
extern bool primerClickLinea;
extern bool primerClickCirculo;
extern int tx0, ty0, tx1, ty1;
extern int clicTriangulo;

extern GLfloat baseColor[4];
extern GLfloat colorSeleccionado[4];
extern Herramienta herramientaActual;

#endif
